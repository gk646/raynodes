#include "application/EditorContext.h"

#include "node/Node.h"
#include "blocks/Connection.h"

namespace {
void AssignConnection(EditorContext& ec, Node& from, Pin& out, Node& to, Pin& in) {
  if(!out.isConnectable(in)) return;
  const auto conn = Connection(from, out, to, in);

  printf("Connection assigned \n");

  ec.core.connections.push_back(conn);

  //Call the event functions
  for (const auto c : from.components) {
    c->onConnectionAdded(ec, conn);
  }
  for (const auto c : to.components) {
    c->onConnectionAdded(ec, conn);
  }
}

void FindDropPin(EditorContext& ec, Node& from, Pin& out) {
  const auto worldMouse = ec.logic.worldMouse;
  const auto radius = Pin::PIN_SIZE /2.0F;
  for (const auto n : ec.core.nodes) {
    //Extended Bound check on the node
    const auto nodeBounds = n->getExtendedBounds(Pin::PIN_SIZE);

    if (CheckCollisionPointRec(worldMouse, nodeBounds)) [[unlikely]] {
      auto posX = n->position.x;
      for (auto* c : n->components) {
        for (auto& p : c->inputs) {
          if (CheckCollisionPointCircle(worldMouse, {posX, (float)p.yPos}, radius)) {
            AssignConnection(ec, from, out, *n, p);
            return;
          }
        }
      }
    }
  }
}

}  // namespace

void Logic::handleDroppedPin(EditorContext& ec) {
  if (!isMakingConnection) return;

  //Cache with correct names
  auto& from = *draggedPinStartNode;
  auto& out = *draggedPin;

  //Reset regardless if a pin is found
  FindDropPin(ec, from, out);

  //Correct order to give strong guarantee when "isMakingConnection" holds
  isMakingConnection = false;
  draggedPin = nullptr;
  draggedPinStartNode = nullptr;
  draggedPinPos = {};
}