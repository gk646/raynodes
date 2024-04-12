#include "application/EditorContext.h"

#include "application/context/ContextCore.h"
#include "node/Node.h"
#include "application/elements/Action.h"

Core::Core() {
  selectedNodes.reserve(200);
  nodeMap.reserve(200);
  nodes.reserve(200);
  addEditorAction(new NewCanvasAction());
}

//-----------NODES-----------//
Node* Core::createNode(EditorContext& ec, NodeType type, Vector2 worldPos) {
  auto newNode = CreateNode(UID, type, worldPos);

  if (!newNode) return nullptr;

  nodes.push_back(newNode);
  nodeMap.insert({UID, newNode});
  UID = NodeID((int)UID + 1);

  for (auto c : newNode->components) {
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
  auto node = nodeMap[id];
  nodeMap.erase(id);
  std::erase(nodes, node);
  for (auto c : node->components) {
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
  if (currentActionIndex
      < static_cast<int>(actionQueue.size()) - 1) {  // Check there's an action to redo
    ++currentActionIndex;                            // Move forward in the action queue
    actionQueue[currentActionIndex]->redo(ec);
  }
}
