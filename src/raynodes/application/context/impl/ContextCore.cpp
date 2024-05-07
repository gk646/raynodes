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
  return true;
}

void Core::resetEditor(EditorContext& ec) {
  hasUnsavedChanges = true;  // Set flag to avoid unnecessary SetTitle insdie addEditorAction()

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

  //Use the hint when provided
  const auto nodeID = hint == UINT16_MAX ? getID() : static_cast<NodeID>(hint);

  Node* newNode = ec.templates.createNode(name, {worldPos.x, worldPos.y}, nodeID);
  if (!newNode) return nullptr;

  // Call event functions
  for (const auto c : newNode->components) {
    c->onCreate(ec, *newNode);
  }

  newNode->onCreation(ec);  // Called after all components

  insertNode(ec, *newNode);

  return newNode;
}

void Core::insertNode(EditorContext& ec, Node& node) {
  nodes.push_back(&node);
  nodeMap.insert({node.uID, &node});
  for (auto* c : node.components) {
    c->onAddedToScreen(ec, node);
  }
}

void Core::removeNode(EditorContext& ec, NodeID id) {
  const auto node = nodeMap[id];
  nodeMap.erase(id);
  std::erase(nodes, node);
  for (const auto c : node->components) {
    c->onRemovedFromScreen(ec, *node);
  }
}

void Core::paste(EditorContext& ec) const {
  if (copiedNodes.empty()) return;
  const Vector2 delta = {ec.logic.worldMouse.x - copiedNodes[0]->x,
                         ec.logic.worldMouse.y - copiedNodes[0]->y};

  const auto action = new NodeCreateAction(static_cast<int>(copiedNodes.size()) + 1);
  // It can happen here that we try to copy deleted nodes
  for (const auto n : copiedNodes) {
    if (n == nullptr) continue;
    auto* newNode = n->clone(ec.core.getID());
    newNode->x += delta.x;
    newNode->y += delta.y;
    action->createdNodes.push_back(newNode);
    ec.core.insertNode(ec, *newNode);
  }
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
      ec.persist.loadFromFile(ec);
      ec.input.consumeKeyboard();
    }
  }
}
void Core::selectAll(EditorContext& ec) {
  for (auto* n : nodes) {
    selectedNodes.insert({n->uID, n});
  }
}
void Core::newFile(EditorContext& ec) {
  if (ec.core.hasUnsavedChanges) ec.ui.showUnsavedChanges = true;
  else {
    ec.core.resetEditor(ec);
    ec.persist.openedFilePath.clear();
    String::updateWindowTitle(ec);
  }
}
void Core::copy(EditorContext& ec) {
  copiedNodes.clear();
  for (const auto& node : selectedNodes | std::views::values) {
    copiedNodes.push_back(node);
  }
}

void Core::addEditorAction(EditorContext& ec, Action* action) {
  if (!action) return;

  // Correctly handle unsaved changes
  // We never unset it even if the user undoes the action - cause its straightforward
  if (!hasUnsavedChanges) {
    hasUnsavedChanges = true;
    String::updateWindowTitle(ec);
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