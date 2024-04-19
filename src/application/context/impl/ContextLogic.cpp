#include "application/EditorContext.h"

#include "node/Node.h"
#include "blocks/Connection.h"

namespace {
void AssignConnection(EditorContext& ec, Node& fromNode, Component& from, OutputPin& out, Node& toNode,
                      Component& to, InputPin& in) {
  if (!out.isConnectable(in)) return;
  const auto conn = new Connection(fromNode, from, out, toNode, to, in);

  printf("Connection assigned \n");

  in.connection = conn;

  ec.core.connections.push_back(conn);

  //Call the event functions
  from.onConnectionAdded(ec, *conn);
  to.onConnectionAdded(ec, *conn);
}

void FindDropPin(EditorContext& ec, Node& fromNode, Component& from, Pin* draggedPin) {
  const auto worldMouse = ec.logic.worldMouse;
  const auto radius = Pin::PIN_SIZE / 2.0F;

  bool isOutputPin = draggedPin->direction == OUTPUT;

  for (const auto n : ec.core.nodes) {
    //Extended Bound check on the node to skip iterating components
    const auto nodeBounds = n->getExtendedBounds(Pin::PIN_SIZE);

    if (CheckCollisionPointRec(worldMouse, nodeBounds)) [[unlikely]] {
      //Allow both types to connect to each other
      if (isOutputPin) {
        auto posX = n->position.x;
        for (auto* to : n->components) {
          for (auto& in : to->inputs) {
            if (CheckCollisionPointCircle(worldMouse, {posX, in.yPos}, radius)) {
              AssignConnection(ec, fromNode, from, *(OutputPin*)draggedPin, *n, *to, in);
              return;
            }
          }
        }
      } else {
        auto posX = n->position.x + n->size.x;
        for (auto* to : n->components) {
          for (auto& out : to->outputs) {
            if (CheckCollisionPointCircle(worldMouse, {posX, out.yPos}, radius)) {
              //Confusing assignment - orders are switched - target node is now output
              //Connection interface is still kept the same for clarity
              AssignConnection(ec, *n, *to, out, fromNode, from, *(InputPin*)draggedPin);
              return;
            }
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
  auto& fromNode = *draggedPinNode;
  auto& from = *draggedPinComponent;

  //Reset regardless if a pin is found
  FindDropPin(ec, fromNode, from, draggedPin);

  //Correct order to give strong guarantee when "isMakingConnection" holds
  isMakingConnection = false;
  draggedPinNode = nullptr;
  draggedPin = nullptr;
  draggedPinComponent = nullptr;
  draggedPinPos = {};
}