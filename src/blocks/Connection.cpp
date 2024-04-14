#include "blocks/Connection.h"

#include <raylib.h>

#include "node/Node.h"

Connection::Connection(Component& from, Pin& out, Component& to, Pin& in)
    : from(from), out(out), to(to), in(in), data() {
  out.connection = this;
  in.connection = this;
}

Vector2 Connection::getFromPos() const {
  //TODO fix / doesnt work if its not the biggest component
  return {from.x + (float)from.width, out.yPos};
}

Vector2 Connection::getToPos() const {
  return {to.x, in.yPos};
}