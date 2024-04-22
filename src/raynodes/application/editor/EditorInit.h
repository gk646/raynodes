// Copyright (c) 2024 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORINIT_H_

namespace Editor {
inline void SetupDisplay(EditorContext& ec) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

  // Initialize the window with the initial size
  SetTraceLogLevel(LOG_WARNING);  //Ive seen this enough
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