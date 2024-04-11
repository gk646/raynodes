#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_

namespace Editor{
inline void SetupDisplay(EditorContext& ec) {
  int m = GetCurrentMonitor();

  const float screenWidth = 1280;
  const float screenHeight = 960;

  ec.display.screenSize.x = screenWidth;
  ec.display.screenSize.y = screenHeight;

  SetTargetFPS(144);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raynodes");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetExitKey(0);
}

inline void SetupCamera(EditorContext& ec) {
  auto& camera = ec.display.camera;
  camera.target = {0.0f, 0.0f};
  camera.offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

}
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_
