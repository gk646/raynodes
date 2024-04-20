#include "Action.h"

#include "application/EditorContext.h"
#include "node/Node.h"

void TextAction::undo(EditorContext& ec) {
  targetText = beforeState;
}

void TextAction::redo(EditorContext& ec) {
  targetText = afterState;
}

//-----------NODE_DELETE-----------//
NodeDeleteAction::NodeDeleteAction(EditorContext& ec, const std::unordered_map<NodeID, Node*>& selectedNodes)
    : Action(DELETE_NODE) {
  deletedNodes.reserve(selectedNodes.size() + 1);
  deletedConnections.reserve(selectedNodes.size() / 5);  //Just guessing

  for (auto [id, node] : selectedNodes) {
    ec.core.removeNode(ec, id);
    ec.core.removeConnections(*node, deletedConnections);
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
    ec.core.connections.push_back(conn);
  }
}

void NodeDeleteAction::redo(EditorContext& ec) {
  removeNodes = true;
  for (const auto n : deletedNodes) {
    ec.core.removeNode(ec, n->id);
  }
  for (const auto conn : deletedConnections) {
    std::erase(ec.core.connections, conn);
  }
}

//-----------NODE_CREATE-----------//
NodeCreateAction::NodeCreateAction(const int size) : Action(CREATE_NODE) {
  createdNodes.reserve(size);
}

NodeCreateAction::~NodeCreateAction() noexcept {
  if (!removeNodes) return;
  for (auto n : createdNodes) {
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
    auto& [x, y] = ec.core.getNode(id)->position;
    x += delta.x;
    y += delta.y;
  }
}

void NodeMovedAction::redo(EditorContext& ec) {
  for (const auto [id, delta] : movedNodes) {
    auto& [x, y] = ec.core.getNode(id)->position;
    x -= delta.x;
    y -= delta.y;
  }
}

float NodeMovedAction::calculateDeltas(EditorContext& ec) {
  auto& selectedNodes = ec.core.selectedNodes;
  for (auto& pair : movedNodes) {
    auto& delta = pair.second;
    const auto current = selectedNodes.at(pair.first)->position;
    delta.x -= current.x;
    delta.y -= current.y;
  }
  // The distance delta is always the same for all nodes (right...)
  // 0 always has to be filled
  return std::abs(movedNodes[0].second.x) + std::abs(movedNodes[0].second.y);
}