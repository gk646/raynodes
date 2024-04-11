#include "Action.h"

#include "shared/rayutils.h"

#include "application/EditorContext.h"
#include "nodes/Nodes.h"

void TextAction::undo(EditorContext& ec) {
  targetText = beforeState;
}
void TextAction::redo(EditorContext& ec) {
  targetText = afterState;
}

//-----------NODE_DELETE-----------//
NodeDeleteAction::NodeDeleteAction(int size) : Action(DELETE_NODE) {
  deletedNodes.reserve(size);
}
NodeDeleteAction::~NodeDeleteAction() noexcept {
  if (!removeNodes) return;
  for (auto n : deletedNodes) {
    delete n;
  }
}
void NodeDeleteAction::undo(EditorContext& ec) {
  removeNodes = false;
  for (auto n : deletedNodes) {
    ec.core.insertNode(n->id, n);
  }
}
void NodeDeleteAction::redo(EditorContext& ec) {
  removeNodes = true;
  for (auto n : deletedNodes) {
    ec.core.removeNode(n->id);
  }
}

//-----------NODE_CREATE-----------//
NodeCreateAction::NodeCreateAction(int size) : Action(CREATE_NODE) {
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
  for (auto n : createdNodes) {
    ec.core.removeNode(n->id);
  }
}
void NodeCreateAction::redo(EditorContext& ec) {
  removeNodes = false;
  for (auto n : createdNodes) {
    ec.core.insertNode(n->id, n);
  }
}

//-----------NODE_MOVE-----------//
NodeMovedAction::NodeMovedAction(int size) : Action(MOVE_NODE) {
  movedNodes.reserve(size);
}
void NodeMovedAction::undo(EditorContext& ec) {
  for (auto n : movedNodes) {
    auto& pos = ec.core.getNode(n.first)->position;
    pos.x += n.second.x;
    pos.y += n.second.y;
  }
}
void NodeMovedAction::redo(EditorContext& ec) {
  for (auto n : movedNodes) {
    auto& pos = ec.core.getNode(n.first)->position;
    pos.x -= n.second.x;
    pos.y -= n.second.y;
  }
}

float NodeMovedAction::calculateDeltas(EditorContext& ec) {
  auto& selectedNodes = ec.core.selectedNodes;
  for (auto& pair : movedNodes) {
    auto& delta = pair.second;
    const auto current = selectedNodes[pair.first]->position;
    delta.x -= current.x;
    delta.y -= current.y;
  }

  //The distance delta is always the same for all nodes (right...)
  //0 always has to be filled
  return movedNodes[0].second.x + movedNodes[0].second.y;
}