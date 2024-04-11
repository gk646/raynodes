#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <vector>
#include <unordered_map>
#include <deque>

struct Core {
  inline static constexpr int targetFPS = 100;
  inline static constexpr int targetLogicTicks = 120;
  inline static constexpr int MAX_ACTIONS = 50;
  std::unordered_map<uint16_t, Node*> selectedNodes;
  std::vector<Node*> nodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  std::deque<Action*> actionQueue;
  NodeID UID = NodeID(1);
  bool logicThreadRunning = true;
  int logicTickTime = 0;
  int drawTickTime = 0;
  int currentActionIndex = -1;

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
  void addEditorAction(Action* action) {
    // If we're not at the end, remove all forward actions
    while (currentActionIndex <  ((int)actionQueue.size() - 1)) {
      actionQueue.pop_back();
    }

    // Add new action and adjust currentActionIndex
    actionQueue.push_back(action);
    currentActionIndex = actionQueue.size() - 1;  // Move to the new action

    // Limit the queue size
    if (actionQueue.size() > 50) {
      actionQueue.pop_front();
      --currentActionIndex;  // Adjust since we removed the oldest action
    }
  }

  void undo();
  void redo();
};
#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
