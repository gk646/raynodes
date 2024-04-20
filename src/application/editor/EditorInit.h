#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_

namespace Editor {
inline void SetupDisplay(EditorContext& ec) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

  // Initialize the window with the initial size
  InitWindow(1280, 720, Info::applicationName);

  // Get the current monitor size
  const int m = GetCurrentMonitor();
  const auto monitorWidth = static_cast<float>(GetMonitorWidth(m));
  const auto monitorHeight = static_cast<float>(GetMonitorHeight(m));

  // Calculate the scale to fit the window in the monitor with some padding
  const float scale = fminf(monitorWidth / 1280.0f, monitorHeight / 720.0f) * 0.75f;

  // Calculate the new window size
  const int screenWidth = static_cast<int>(1280.0f * scale);
  const int screenHeight = static_cast<int>(720.0f * scale);

  ec.display.screenSize.x = static_cast<float>(screenWidth);
  ec.display.screenSize.y = static_cast<float>(screenHeight);

  // Set the new window size
  SetWindowSize(screenWidth, screenHeight);

  // Center the window on the screen
  SetWindowPosition((monitorWidth - screenWidth) / 2, (monitorHeight - screenHeight) / 2);

  SetTargetFPS(Core::TARGET_FPS);
  SetExitKey(0);
  SetTraceLogLevel(LOG_WARNING);
  DisableEventWaiting();
}

inline void SetupCamera(EditorContext& ec) {
  auto& [offset, target, rotation, zoom] = ec.display.camera;
  target = {0.0f, 0.0f};
  offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};
  rotation = 0.0f;
  zoom = 1.0f;
}

}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_