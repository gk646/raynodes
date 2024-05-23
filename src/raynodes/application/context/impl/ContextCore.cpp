// Copyright (c) 2024 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <ranges>
#include <tinyfiledialogs.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

struct NodeIDs {
  NodeID original;
  NodeID copied;
};

// Helps to copy relative connection between copied nodes
struct ConnectionHelper {
  static constexpr int LIMIT = 300;  // Support up to 300 nodes for relative connection copying
  int size = 0;
  std::array<NodeIDs, LIMIT> mapping{};
  [[nodiscard]] NodeID getOriginalID(const NodeID id) const {
    for (const auto entry : mapping) {
      if (entry.original == id) return entry.copied;
    }
    return NodeID(UINT16_MAX);
  }
  void add(const NodeID original, const NodeID copied) {
    if (size < LIMIT) { mapping[size++] = {original, copied}; }
  }
  static Int2 GetIndices(Node& n, const Component* c, const Pin* p) {
    if (c == nullptr) {
      for (int i = 0; i < n.outputs.size(); ++i) {
        if (&n.outputs[i] == p) return {-1, i};
      }
      if (&n.nodeIn == p) return {-1, 0};
    }

    for (int comp = 0; comp < n.components.size(); ++comp) {
      for (int in = 0; in < n.components[comp]->inputs.size(); ++in) {
        if (&n.components[comp]->inputs[in] == p) { return {comp, in}; }
      }

      for (int out = 0; out < n.components[comp]->outputs.size(); ++out) {
        if (&n.components[comp]->outputs[out] == p) { return {comp, out}; }
      }
    }

    return {-1, -1};
  }
  static void addConnection(EditorContext& ec, const NodeID newTo, const Connection* oldConn, const NodeID newFrom) {
    Node* toNode = ec.core.getNode(newTo);
    const auto toIndices = GetIndices(oldConn->toNode, oldConn->to, &oldConn->in);

    Component* to;
    InputPin* in;
    if (toIndices.x == -1) {
      to = nullptr;
      in = &toNode->nodeIn;
    } else {
      to = toNode->components[toIndices.x];
      in = &to->inputs[toIndices.y];
    }

    Node& fromNode = *ec.core.getNode(newFrom);
    const auto fromIndicies = GetIndices(oldConn->fromNode, oldConn->from, &oldConn->out);

    Component* from;
    OutputPin* out;
    if (fromIndicies.x == -1) {
      from = nullptr;
      out = &fromNode.outputs[fromIndicies.y];
    } else {
      from = fromNode.components[fromIndicies.x];
      out = &from->outputs[fromIndicies.y];
    }

    ec.core.addConnection(new Connection(fromNode, from, *out, *toNode, to, *in));
  }
  void addConnections(EditorContext& ec) const {
    for (int i = 0; i < size; ++i) {
      const auto node = ec.core.getNode(mapping[i].original);
      if (node == nullptr) continue;  // Pasting delete nodes
      for (const auto comp : node->components) {
        for (const auto& in : comp->inputs) {
          const auto conn = in.connection;
          if (conn != nullptr) {
            const auto newFrom = getOriginalID(conn->fromNode.uID);
            if (newFrom != UINT16_MAX) {
              const auto newID = getOriginalID(node->uID);
              addConnection(ec, newID, conn, newFrom);
            }
          }
        }
      }
    }
  }
  void reset() { size = 0; }
};

static ConnectionHelper CONNECTION_HELPER;

bool Core::loadCore(EditorContext& ec) {
  hasUnsavedChanges = true;                    // Set flag to avoid unnecessary SetTitle
  addEditorAction(ec, new NewCanvasAction());  // Add first dummy action
  hasUnsavedChanges = false;                   // Reset flag after dummy

  // Reserve healthy amount
  selectedNodes.reserve(200);
  nodeMap.reserve(200);
  nodes.reserve(200);
  copiedNodes.reserve(200);
  connections.reserve(MAX_ACTIONS + 1);
  nodeGroups.reserve(5);
  return true;
}
void Core::resetEditor(EditorContext& ec) {
  hasUnsavedChanges = true;  // Set flag to avoid unnecessary SetTitle insdie addEditorAction()

  nodeGroups.clear();

  // Setup action queue
  for (auto a : actionQueue) {
    delete a;
  }

  actionQueue.clear();

  addEditorAction(ec, new NewCanvasAction());

  // Setup data holders
  selectedNodes.clear();
  nodeMap.clear();
  for (auto n : nodes) {
    delete n;
  }
  nodes.clear();

  copiedNodes.clear();

  for (auto conn : connections) {
    delete conn;
  }
  connections.clear();
  UID = static_cast<NodeID>(0);

  hasUnsavedChanges = false;
}

Node* Core::createNode(EditorContext& ec, const char* name, const Vector2 worldPos, uint16_t hint) {
  if (name == nullptr) return nullptr;

  //Use the hint when provided
  const auto nodeID = hint == UINT16_MAX ? getID() : static_cast<NodeID>(hint);

  // Calls event function internally
  Node* newNode = ec.templates.createNode(ec, name, {worldPos.x, worldPos.y}, nodeID);
  if (!newNode) return nullptr;

  insertNode(ec, *newNode);

  return newNode;
}
void Core::insertNode(EditorContext& ec, Node& node) {
  if (nodeMap.contains(node.uID)) return;
  nodes.push_back(&node);
  nodeMap.insert({node.uID, &node});

  for (auto* c : node.components) {
    c->onAddedToScreen(ec, node);
  }
}
// Only call with a valid id
void Core::removeNode(EditorContext& ec, const NodeID id) {
  if (!nodeMap.contains(id)) return;
  const auto node = nodeMap[id];
  nodeMap.erase(id);
  std::erase(nodes, node);

  for (const auto c : node->components) {
    c->onRemovedFromScreen(ec, *node);
  }
}

void Core::paste(EditorContext& ec) const {
  if (copiedNodes.empty()) return;
  const Vector2 delta = {ec.logic.worldMouse.x - copiedNodes[0]->x, ec.logic.worldMouse.y - copiedNodes[0]->y};

  const auto action = new NodeCreateAction(static_cast<int>(copiedNodes.size()) + 1);
  CONNECTION_HELPER.reset();

  for (const auto n : copiedNodes) {
    if (n == nullptr) continue;  // It can happen here that we try to copy deleted nodes

    auto* newNode = n->clone(ec.core.getID());
    newNode->x += delta.x;
    newNode->y += delta.y;

    action->createdNodes.push_back(newNode);
    ec.core.insertNode(ec, *newNode);

    CONNECTION_HELPER.add(n->uID, newNode->uID);  // Save the mapping from old->new
  }

  CONNECTION_HELPER.addConnections(ec);

  ec.core.addEditorAction(ec, action);
}
void Core::cut(EditorContext& ec) {
  if (selectedNodes.empty()) return;
  copiedNodes.clear();
  for (const auto n : selectedNodes | std::views::values) {
    copiedNodes.push_back(n);
  }
  const auto action = new NodeDeleteAction(ec, selectedNodes);
  ec.core.addEditorAction(ec, action);
  selectedNodes.clear();
}
void Core::erase(EditorContext& ec) {
  if (selectedNodes.empty()) return;
  const auto action = new NodeDeleteAction(ec, selectedNodes);
  ec.core.addEditorAction(ec, action);
  selectedNodes.clear();
}

void Core::open(EditorContext& ec) {
  if (ec.core.hasUnsavedChanges) ec.ui.showUnsavedChanges = true;
  else {
    auto* res = tinyfd_openFileDialog("Open File", nullptr, 1, Info::fileFilter, Info::fileDescription, 0);
    if (res != nullptr) {
      ec.persist.openedFilePath = res;
      ec.persist.importProject(ec);
      ec.input.consumeKeyboard();
    }
  }
}
void Core::selectAll(EditorContext& /**/) {
  for (auto* n : nodes) {
    selectedNodes.insert({n->uID, n});
  }
}
void Core::newFile(EditorContext& ec) {
  if (ec.core.hasUnsavedChanges) ec.ui.showUnsavedChanges = true;
  else {
    ec.core.resetEditor(ec);
    ec.persist.openedFilePath.clear();
    ec.string.updateWindowTitle(ec);
  }
}
void Core::copy(EditorContext& /**/) {
  copiedNodes.clear();
  for (const auto& node : selectedNodes | std::views::values) {
    copiedNodes.push_back(node);
  }
}

void Core::addEditorAction(EditorContext& ec, Action* action) {
  if (!action) return;

  // We never unset it even if the user undoes the action - cause its straightforward
  if (hasUnsavedChanges == false) {
    hasUnsavedChanges = true;
    ec.string.updateWindowTitle(ec);
  }

  // If we're not at the end, remove all forward actions
  while (currentActionIndex < static_cast<int>(actionQueue.size()) - 1) {
    delete actionQueue.back();  //Delete ptr
    actionQueue.pop_back();
  }

  actionQueue.push_back(action);
  currentActionIndex = static_cast<int>(actionQueue.size()) - 1;  // Move to the new action

  // Limit the queue size
  if (actionQueue.size() > MAX_ACTIONS) {
    delete actionQueue.front();  //Delete ptr
    actionQueue.pop_front();
    --currentActionIndex;
  }
}
void Core::undo(EditorContext& ec) {
  hasUnsavedChanges = true;  // Just set the flag for safety
  if (currentActionIndex >= 1) {
    // Check there's an action to undo
    actionQueue[currentActionIndex]->undo(ec);
    --currentActionIndex;  // Move back in the action queue
  }
}
void Core::redo(EditorContext& ec) {
  if (currentActionIndex < static_cast<int>(actionQueue.size()) - 1) {
    // Check there's an action to redo
    ++currentActionIndex;  // Move forward in the action queue
    actionQueue[currentActionIndex]->redo(ec);
  }
}