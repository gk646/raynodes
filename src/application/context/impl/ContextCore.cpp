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
//

#include "application/EditorContext.h"

#include "node/Node.h"
#include "application/context/ContextCore.h"
#include "application/elements/Action.h"

#include <shared/fwd.h>

Core::Core() {
  selectedNodes.reserve(200);
  nodeMap.reserve(200);
  nodes.reserve(200);
  copiedNodes.reserve(200);
  connections.reserve(50);
  addEditorAction(new NewCanvasAction());
}

//-----------NODES-----------//
Node* Core::createNode(EditorContext& ec, const NodeType type, const Vector2 worldPos, uint16_t hint) {
  //Use the hint when provided
  auto newNode = CreateNode(hint == 0 ? UID : static_cast<NodeID>(hint), type, worldPos);

  if (!newNode) return nullptr;

  nodes.push_back(newNode);
  nodeMap.insert({newNode->id, newNode});
  UID = getNextID();

  for (const auto c : newNode->components) {
    c->onCreate(ec, *newNode);
  }

  return newNode;
}
void Core::insertNode(EditorContext& ec, NodeID id, Node* node) {
  nodes.push_back(node);
  nodeMap.insert({id, node});
  for (auto c : node->components) {
    c->onAddedToScreen(ec, *node);
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
//-----------ACTIONS-----------//
void Core::addEditorAction(Action* action) {
  // If we're not at the end, remove all forward actions
  while (currentActionIndex < ((int)actionQueue.size() - 1)) {
    delete actionQueue.back();  //Delete ptr
    actionQueue.pop_back();
  }

  actionQueue.push_back(action);
  currentActionIndex = actionQueue.size() - 1;  // Move to the new action

  // Limit the queue size
  if (actionQueue.size() > 50) {
    delete actionQueue.front();  //Delete ptr
    actionQueue.pop_front();
    --currentActionIndex;
  }
}
void Core::undo(EditorContext& ec) {
  if (currentActionIndex >= 1) {  // Check there's an action to undo
    actionQueue[currentActionIndex]->undo(ec);
    --currentActionIndex;  // Move back in the action queue
  }
}
void Core::redo(EditorContext& ec) {
  if (currentActionIndex < static_cast<int>(actionQueue.size()) - 1) {  // Check there's an action to redo
    ++currentActionIndex;                                               // Move forward in the action queue
    actionQueue[currentActionIndex]->redo(ec);
  }
}