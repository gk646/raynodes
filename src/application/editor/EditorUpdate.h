#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_

namespace Editor {
inline void StartUpdateTick(EditorContext& ec) {
  auto& camera = ec.display.camera;

  ec.display.screenSize.x = GetScreenWidth();
  ec.display.screenSize.y = GetScreenHeight();

  camera.offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};

  float val = GetMouseWheelMove();
  camera.zoom += GetMouseWheelMove() * 0.05f;
  camera.zoom = (camera.zoom > 3.0f) ? 3.0f : camera.zoom;
  camera.zoom = (camera.zoom < 0.1f) ? 0.1f : camera.zoom;


  auto mouse = GetMousePosition();
  ec.logic.worldMouse = GetScreenToWorld2D(mouse, camera);
  ec.logic.mouse = mouse;
}

}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
