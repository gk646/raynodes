#include "EditorContext.h"

#include <nodes/Nodes.h>

EditorContext::EditorContext() {
  nodes.reserve(200);
}

//-----------NODES-----------//

void EditorContext::createNode(NodeType type, Vector2 pos) {
  auto worldPos = GetScreenToWorld2D(pos, camera);

  auto newNode = CreateNode(UID, type, worldPos);

  if (!newNode) return;

  nodes.push_back(newNode);
  nodeMap.insert({UID, newNode});

  UID = NodeID((int)UID + 1);
  printf("Created Node %d\n", (int)type);
}

void EditorContext::deleteNode(NodeID id) {
  auto node = nodeMap[id];
  nodeMap.erase(id);
  std::erase(nodes, node);
  delete node;
}

Node* EditorContext::getNode(NodeID id) {
  return nodeMap[id];
}
