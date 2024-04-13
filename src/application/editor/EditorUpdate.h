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
    auto& selectRect = ec.logic.selectRect;
    auto& selectPoint = ec.logic.selectPoint;
    const auto worldMouse = ec.logic.worldMouse;

    if (worldMouse.x < selectPoint.x) {
      if (worldMouse.y > selectPoint.y) {
        // Mouse is left bottom
        selectRect.x = worldMouse.x;
        selectRect.y = worldMouse.y - (worldMouse.y - selectPoint.y);
        selectRect.width = selectPoint.x - worldMouse.x;
        selectRect.height = worldMouse.y - selectPoint.y;
      } else {
        // Mouse is left top
        selectRect.x = worldMouse.x;
        selectRect.y = worldMouse.y;
        selectRect.width = selectPoint.x - worldMouse.x;
        selectRect.height = selectPoint.y - worldMouse.y;
      }
    } else {
      if (worldMouse.y > selectPoint.y) {
        // Mouse is right bottom
        selectRect.x = selectPoint.x;
        selectRect.y = selectPoint.y;
        selectRect.width = worldMouse.x - selectPoint.x;
        selectRect.height = worldMouse.y - selectPoint.y;
      } else {
        // Mouse is right top
        selectRect.x = selectPoint.x;
        selectRect.y = selectPoint.y - (selectPoint.y - worldMouse.y);
        selectRect.width = worldMouse.x - selectPoint.x;
        selectRect.height = selectPoint.y - worldMouse.y;
      }
    }
  }
}
}  // namespace
namespace Editor {
inline void UpdateTick(EditorContext& ec) {
  StartUpdateTick(ec);

  // Critical section
  ec.core.lock();
  {
    auto& nodes = ec.core.nodes;
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
      (*it)->update(ec);
    }
  }
  ec.core.unlock();

  if (ec.logic.isSelecting) {
    FormatSelectRectangle(ec);
  }
}
}  // namespace Editor
#endif  // RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
