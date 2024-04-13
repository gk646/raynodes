#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <atomic>
#include <deque>
#include <unordered_map>
#include <vector>

struct Core {
  inline static constexpr int TARGET_FPS = 100;
  inline static constexpr int targetLogicTicks = 120;
  inline static constexpr int MAX_ACTIONS = 50;

  std::unordered_map<NodeID, Node*> selectedNodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  std::deque<Action*> actionQueue;
  std::vector<Node*> nodes;
  std::vector<Connection> connections;
  std::atomic_flag flag{};  //Not needed currently
  int logicTickTime = 0;    //Not needed
  int drawTickTime = 0;
  int currentActionIndex = -1;
  NodeID UID = NodeID(1);
  bool logicThreadRunning = true;

  Core();

  Node* createNode(EditorContext& ec, NodeType type, Vector2 worldPos);
  void insertNode(EditorContext& ec, NodeID id, Node* node);
  void removeNode(EditorContext& ec, NodeID id);
  inline Node* getNode(NodeID id) { return nodeMap[id]; }
  inline void moveToFront(Node* node) {
    std::erase(nodes, node);
    nodes.push_back(node);
  }
  inline void lock() {
    while (flag.test_and_set(std::memory_order_acquire)) {}
  }
  inline void unlock() { flag.clear(std::memory_order_release); }
  void addEditorAction(Action* action);
  void undo(EditorContext& ec);
  void redo(EditorContext& ec);
};
#endif  // RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
