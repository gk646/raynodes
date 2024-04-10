#ifndef RAYNODES_SRC_GRAPHICS_COMPONENTRENDERER_H_
#define RAYNODES_SRC_GRAPHICS_COMPONENTRENDERER_H_

#include <raylib.h>

struct DrawResource {
  float fontSize;
  Vector2 mousePos;
  Vector2 worldMouse;
  Font font;
};

struct UpdateResource {
  Vector2 worldMouse;
};

namespace cr {
template <class T>
void ContextMenu();
void DrawGrid(const Camera2D& camera, float gridSpacing);

}  // namespace cr

#endif  //RAYNODES_SRC_GRAPHICS_COMPONENTRENDERER_H_
