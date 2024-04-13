#include "application/EditorContext.h"

#include "node/Node.h"
#include "blocks/Connection.h"

void Logic::handleDroppedPin(EditorContext& ec) {
  if (!isMakingConnection) return;

  //Connection failed
  if (hoveredNode != nullptr) {
    //This is done because saving pin dimension costs space
    //They are all placed deterministic at the bottom of the node
    //Exactly as in Node::draw()
    auto& hNode = *hoveredNode;
    const float startX = hNode.position.x + Node::PADDING;
    const float startY =
        hNode.position.y + Node::PADDING + Node::OFFSET_Y + hNode.contentHeight + 10;

    auto bounds = Rectangle{startX, startY, Pin::PIN_SIZE, Pin::PIN_SIZE};

    //Cache with correct names
    auto& from = *draggedPinStartNode;
    auto& out = *draggedPin;

    for (auto& pin : hNode.pins) {
      if (CheckCollisionPointRec(worldMouse, bounds)) {
        //Create the connection
        //Assigns the data pointers internally to pins
        auto conn = Connection(from, out, hNode, pin);
        ec.core.connections.push_back(conn);

        //Call the event functions
        for (auto c : from.components) {
          c->onConnectionAdded(ec, conn);
        }
        for (auto c : hNode.components) {
          c->onConnectionAdded(ec, conn);
        }

        break;  //Done
      }
    }
  }

  //Correct order to give strong guarantee when "isMakingConnection" holds
  isMakingConnection = false;
  draggedPin = nullptr;
  draggedPinStartNode = nullptr;
  draggedPinPos = {};
}