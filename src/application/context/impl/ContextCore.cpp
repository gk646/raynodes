#include "application/EditorContext.h"

#include "application/context/ContextCore.h"
#include "nodes/Nodes.h"
#include "application/elements/Action.h"

Node* Core::createNode(NodeType type, Vector2 worldPos) {
  auto newNode = CreateNode(UID, type, worldPos);

  if (!newNode) return nullptr;

  nodes.push_back(newNode);
  nodeMap.insert({UID, newNode});

  UID = NodeID((int)UID + 1);
  return newNode;
}

void Core::undo() {
  if (currentActionIndex < static_cast<int>(actionQueue.size()) - 1) {
    ++currentActionIndex;
    actionQueue[currentActionIndex]->redo();
  }

}
void Core::redo() {
  if (currentActionIndex >= 0) {
    actionQueue[currentActionIndex]->undo();
    --currentActionIndex;
  }
}
