#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <vector>
#include <unordered_map>
#include <stack>

struct Core {
  std::unordered_map<uint16_t, Node*> selectedNodes;
  std::vector<Node*> nodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  std::stack<Action*> actionStack;
  NodeID UID = NodeID(1);

  Core() {
    selectedNodes.reserve(200);
    nodeMap.reserve(200);
    nodes.reserve(200);
  };
  Node* createNode(NodeType type, Vector2 worldPos);
  inline void insertNode(NodeID id, Node* node) {
    nodes.push_back(node);
    nodeMap.insert({id, node});
  }
  inline void removeNode(NodeID id) {
    auto node = nodeMap[id];
    nodeMap.erase(id);
    std::erase(nodes, node);
  }
  inline Node* getNode(NodeID id) { return nodeMap[id]; }
};
#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
