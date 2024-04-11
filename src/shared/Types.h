#ifndef RAYNODES_SRC_TYPES_TYPES_H_
#define RAYNODES_SRC_TYPES_TYPES_H_

#include <unordered_map>
#include <raylib.h>

#include "shared/fwd.h"

struct DrawResource {
  float fontSize;
  Vector2 mousePos;
  Vector2 worldMouse;
  Font font;
};

struct UpdateResource {
  Rectangle selectRect;
  Vector2 worldMouse;
  bool anyNodeHovered;
  std::unordered_map<uint16_t, Node*>* selectedNodes;
  bool isSelecting;
  bool isDraggingNode;
};

struct Point {
  float x;
  float y;
};


#endif  //RAYNODES_SRC_TYPES_TYPES_H_
