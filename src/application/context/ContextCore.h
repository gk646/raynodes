#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <atomic>
#include <deque>
#include <unordered_map>
#include <vector>

struct Core
{
  inline static constexpr int targetFPS = 100;
  inline static constexpr int targetLogicTicks = 120;
  inline static constexpr int MAX_ACTIONS = 50;

  std::unordered_map<NodeID, Node*> selectedNodes;
  std::vector<Node*> nodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  bool logicThreadRunning = true;
  int logicTickTime = 0;
  int drawTickTime = 0;
  int currentActionIndex = -1;
  std::deque<Action*> actionQueue;
  NodeID UID = NodeID(1);
  std::atomic_flag flag{};

  Core();

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

  inline void moveToFront(Node* node) {
    std::erase(nodes, node);
    nodes.push_back(node);
  }

  inline void lock() {
    while (flag.test_and_set(std::memory_order_acquire))
      {}
  }

  inline void unlock() { flag.clear(std::memory_order_release); }

  void addEditorAction(Action* action);
  void undo(EditorContext& ec);
  void redo(EditorContext& ec);
};
#endif  // RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
