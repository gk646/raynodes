#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_

namespace {
void StartUpdateTick(EditorContext& ec) {
  auto& camera = ec.display.camera;

  ec.display.screenSize.x = static_cast<float>(GetScreenWidth());
  ec.display.screenSize.y = static_cast<float>(GetScreenHeight());

  camera.offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};

  camera.zoom += GetMouseWheelMove() * 0.05f;
  camera.zoom = (camera.zoom > 3.0f) ? 3.0f : camera.zoom;
  camera.zoom = (camera.zoom < 0.1f) ? 0.1f : camera.zoom;

  const auto mouse = GetMousePosition();
  ec.logic.worldMouse = GetScreenToWorld2D(mouse, camera);
  ec.logic.mouse = mouse;
  ec.logic.isAnyNodeHovered = false;  // Reset each tick
  ec.input.reset();                   //Reset input for tick
}
void FormatSelectRectangle(EditorContext& ec) {
  {
    auto& [x, y, width, height] = ec.logic.selectRect;
    auto& selectPoint = ec.logic.selectPoint;
    const auto worldMouse = ec.logic.worldMouse;

    if (worldMouse.x < selectPoint.x) {
      if (worldMouse.y > selectPoint.y) {
        // Mouse is left bottom
        x = worldMouse.x;
        y = worldMouse.y - (worldMouse.y - selectPoint.y);
        width = selectPoint.x - worldMouse.x;
        height = worldMouse.y - selectPoint.y;
      } else {
        // Mouse is left top
        x = worldMouse.x;
        y = worldMouse.y;
        width = selectPoint.x - worldMouse.x;
        height = selectPoint.y - worldMouse.y;
      }
    } else {
      if (worldMouse.y > selectPoint.y) {
        // Mouse is right bottom
        x = selectPoint.x;
        y = selectPoint.y;
        width = worldMouse.x - selectPoint.x;
        height = worldMouse.y - selectPoint.y;
      } else {
        // Mouse is right top
        x = selectPoint.x;
        y = selectPoint.y - (selectPoint.y - worldMouse.y);
        width = worldMouse.x - selectPoint.x;
        height = selectPoint.y - worldMouse.y;
      }
    }
  }
}
}  // namespace
namespace Editor {
inline void UpdateTick(EditorContext& ec) {
  StartUpdateTick(ec);

  //Reverse update to correctly reflect input layers
  auto& nodes = ec.core.nodes;
  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    (*it)->update(ec);
  }

  if (ec.logic.isSelecting) {
    FormatSelectRectangle(ec);
  }
}
}  // namespace Editor
#endif  // RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_