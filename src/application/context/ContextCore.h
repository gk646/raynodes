#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <atomic>
#include <deque>
#include <unordered_map>
#include <vector>
#include <shared/fwd.h>

struct Core {
  static constexpr int TARGET_FPS = 100;
  static constexpr int targetLogicTicks = 120;
  static constexpr int MAX_ACTIONS = 50;

  std::unordered_map<NodeID, Node*> selectedNodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  std::deque<Action*> actionQueue;
  std::vector<Node*> nodes;
  std::vector<Node*> copiedNodes;
  std::vector<Connection*> connections;
  int drawTickTime = 0;
  int currentActionIndex = -1;
  NodeID UID = static_cast<NodeID>(1);
  bool logicThreadRunning = true;

  Core();
  auto getNextID() -> NodeID {
    UID = static_cast<NodeID>(UID + 1);
    return UID;
  }
  auto createNode(EditorContext& ec, NodeType type, Vector2 worldPos, uint16_t hint = 0) -> Node*;
  auto getNode(const NodeID id) -> Node* { return nodeMap[id]; }
  void insertNode(EditorContext& ec, NodeID id, Node* node);
  void removeNode(EditorContext& ec, NodeID id);
  void moveToFront(Node* node) {
    std::erase(nodes, node);
    nodes.push_back(node);
  }
  void removeConnections(Node& node, std::vector<Connection*>& collector) {
    const auto newEnd =
        std::remove_if(connections.begin(), connections.end(), [&node, &collector](Connection* conn) {
          if (&conn->fromNode == &node || &conn->toNode == &node) {
            collector.push_back(conn);
            return true;
          }
          return false;
        });

    // Erase the removed elements
    connections.erase(newEnd, connections.end());
  }
  //-------------EditorActions--------------//
  void addEditorAction(Action* action);
  void undo(EditorContext& ec);
  void redo(EditorContext& ec);
};
#endif  // RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_