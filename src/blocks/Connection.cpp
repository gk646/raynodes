#include "blocks/Connection.h"

#include <raylib.h>

#include "node/Node.h"

Vector2 Connection::getFromPos() const {
  return from.getPinPosition(out.idx);
}

Vector2 Connection::getToPos() const {
  return to.getPinPosition(in.idx);
}