#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_

#include "nodes/Nodes.h"

namespace {
inline void StartUpdateTick(EditorContext& ec) {
  auto& camera = ec.display.camera;

  ec.display.screenSize.x = GetScreenWidth();
  ec.display.screenSize.y = GetScreenHeight();

  camera.offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};

  camera.zoom += GetMouseWheelMove() * 0.05f;
  camera.zoom = (camera.zoom > 3.0f) ? 3.0f : camera.zoom;
  camera.zoom = (camera.zoom < 0.1f) ? 0.1f : camera.zoom;

  auto mouse = GetMousePosition();
  ec.logic.worldMouse = GetScreenToWorld2D(mouse, camera);
  ec.logic.mouse = mouse;
}
}  // namespace

namespace Editor {

inline void UpdateTick(EditorContext& ec) {
  StartUpdateTick(ec);
  UpdateResource ur = ec.getUpdateResource();

  for (auto c : ec.core.nodes) {
    c->update(ur);
  }

  ec.logic.isAnyNodeHovered = ur.anyNodeHovered;
  ec.logic.isDraggingNode = ur.isDraggingNode;
}

}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
