#include "application/EditorContext.h"
#include "nodes/Nodes.h"
#include "application/context/ContextCore.h"

Node* Core::createNode(NodeType type, Vector2 worldPos) {
  auto newNode = CreateNode(UID, type, worldPos);

  if (!newNode) return nullptr;

  nodes.push_back(newNode);
  nodeMap.insert({UID, newNode});

  UID = NodeID((int)UID + 1);
  return newNode;
}
