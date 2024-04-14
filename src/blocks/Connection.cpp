#include "blocks/Connection.h"

#include <raylib.h>

#include "node/Node.h"


Vector2 Connection::getFromPos() const {
  return {from.position.x+from.size.x,out.yPos};
}

Vector2 Connection::getToPos() const {
  return {to.position.x ,in.yPos};
}