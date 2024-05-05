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

#include "Action.h"

#include "application/EditorContext.h"
#include "node/Node.h"

void TextAction::undo(EditorContext& /**/) {
  targetText = beforeState;
}

void TextAction::redo(EditorContext& /**/) {
  targetText = afterState;
}

//-----------NODE_DELETE-----------//
NodeDeleteAction::NodeDeleteAction(EditorContext& ec, const std::unordered_map<NodeID, Node*>& selectedNodes)
    : Action(DELETE_NODE) {
  deletedNodes.reserve(selectedNodes.size() + 1);
  deletedConnections.reserve(selectedNodes.size() / 5);  //Just guessing

  for (auto [id, node] : selectedNodes) {
    ec.core.removeNode(ec, id);
    ec.core.removeConnectionsFromNode(*node, deletedConnections);
    deletedNodes.push_back(node);
  }
}

NodeDeleteAction::~NodeDeleteAction() noexcept {
  if (!removeNodes) return;
  for (const auto n : deletedNodes) {
    delete n;
  }
  for (const auto conn : deletedConnections) {
    delete conn;
  }
}

void NodeDeleteAction::undo(EditorContext& ec) {
  removeNodes = false;
  for (const auto n : deletedNodes) {
    ec.core.insertNode(ec, n->id, n);
  }
  for (const auto conn : deletedConnections) {
    ec.core.addConnection(conn);
  }
}

void NodeDeleteAction::redo(EditorContext& ec) {
  removeNodes = true;
  for (const auto n : deletedNodes) {
    ec.core.removeNode(ec, n->id);
  }
  for (const auto conn : deletedConnections) {
    ec.core.removeConnection(conn);
  }
}

//-----------NODE_CREATE-----------//
NodeCreateAction::NodeCreateAction(const int size) : Action(CREATE_NODE) {
  createdNodes.reserve(size);
}

NodeCreateAction::~NodeCreateAction() noexcept {
  if (!removeNodes) return;
  for (const auto n : createdNodes) {
    delete n;
  }
}

void NodeCreateAction::undo(EditorContext& ec) {
  removeNodes = true;
  for (const auto n : createdNodes) {
    ec.core.removeNode(ec, n->id);
  }
}

void NodeCreateAction::redo(EditorContext& ec) {
  removeNodes = false;
  for (const auto n : createdNodes) {
    ec.core.insertNode(ec, n->id, n);
  }
}

//-----------NODE_MOVE-----------//
NodeMovedAction::NodeMovedAction(const int size) : Action(MOVE_NODE) {
  movedNodes.reserve(size);
}

void NodeMovedAction::undo(EditorContext& ec) {
  for (const auto [id, delta] : movedNodes) {
    const auto node = ec.core.getNode(id);
    node->x += delta.x;
    node->y += delta.y;
  }
}

void NodeMovedAction::redo(EditorContext& ec) {
  for (const auto [id, delta] : movedNodes) {
    const auto node = ec.core.getNode(id);
    node->x -= delta.x;
    node->y -= delta.y;
  }
}

float NodeMovedAction::calculateDeltas(const EditorContext& ec) {
  auto& selectedNodes = ec.core.selectedNodes;
  for (auto& [id, delta] : movedNodes) {
    const auto node = selectedNodes.at(id);
    delta.x -= node->x;
    delta.y -= node->y;
  }
  // The distance delta is always the same for all nodes (right...)
  // 0 always has to be filled
  return std::abs(movedNodes[0].second.x) + std::abs(movedNodes[0].second.y);
}

//-----------CONNECTION_DELETE-----------//
ConnectionDeleteAction::ConnectionDeleteAction(const int size) : Action(CONNECTION_DELETED) {
  deletedConnections.reserve(size);
}

ConnectionDeleteAction::~ConnectionDeleteAction() noexcept {
  if (!removeNodes) return;
  for (const auto n : deletedConnections) {
    delete n;
  }
}

void ConnectionDeleteAction::undo(EditorContext& ec) {
  removeNodes = false;
  for (const auto conn : deletedConnections) {
    ec.core.addConnection(conn);
  }
}

void ConnectionDeleteAction::redo(EditorContext& ec) {
  removeNodes = true;
  for (const auto conn : deletedConnections) {
    ec.core.removeConnection(conn);
  }
}