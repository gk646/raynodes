#include "blocks/Connection.h"

#include <raylib.h>

#include "node/Node.h"

Connection::Connection(Node& fromNode, Component& from, OutputPin& out, Node& toNode, Component& to,
                       InputPin& in)
    : fromNode(fromNode), from(from), out(out), toNode(toNode), to(to), in(in) {
  in.connection = this;
}

Vector2 Connection::getFromPos() const {
  return {fromNode.position.x + fromNode.size.x, out.yPos};
}

Vector2 Connection::getToPos() const {
  return {to.x, in.yPos};
}