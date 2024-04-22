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

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTCORE_H_

#include <deque>
#include <unordered_map>
#include <vector>


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
  void removeConnection(Connection* conn) {
    conn->close();
    std::erase(connections, conn);
  }
  void addConnection(Connection* conn) {
    conn->open();
    connections.push_back(conn);
  }
  //Optimized delete method to remove multiple connections
  void removeConnectionsFromNode(Node& node, std::vector<Connection*>& collector) {
    const auto newEnd =
        std::remove_if(connections.begin(), connections.end(), [&node, &collector](Connection* conn) {
          if (&conn->fromNode == &node || &conn->toNode == &node) {
            conn->close();
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