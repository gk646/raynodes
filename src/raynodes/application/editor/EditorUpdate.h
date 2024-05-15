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

#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_

namespace {
void FormatSelectRectangle(EditorContext& ec) {
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
}  // namespace

namespace Editor {
inline void UpdateTick(EditorContext& ec) {
  //Reverse update to correctly reflect input layers
  for (auto it = ec.core.nodes.rbegin(); it != ec.core.nodes.rend(); ++it) {
    Node::Update(ec, **it);
  }

  if (ec.logic.isSelecting) { FormatSelectRectangle(ec); }
}
inline void StartUpdateTick(EditorContext& ec) {
  auto& camera = ec.display.camera;

  const auto currScreenWidth = static_cast<float>(GetScreenWidth());
  const auto currScreenHeight = static_cast<float>(GetScreenHeight());

  if (ec.display.screenSize.x != currScreenWidth || ec.display.screenSize.y != currScreenHeight) {
    UnloadRenderTexture(ec.display.uiTexture);
    ec.display.uiTexture = LoadRenderTexture(static_cast<int>(currScreenWidth), static_cast<int>(currScreenHeight));
  }
  ec.display.screenSize.x = currScreenWidth;
  ec.display.screenSize.y = currScreenHeight;

  camera.offset = {ec.display.screenSize.x / 2.0f, ec.display.screenSize.y / 2.0f};

  // Scale to normal screen space
  const auto mouse = GetMousePosition();

  ec.logic.worldMouse = GetScreenToWorld2D(mouse, camera);
  ec.logic.mouse = mouse;
  ec.logic.isAnyNodeHovered = false;  // Reset each tick
  ec.logic.isAnyNodeDragged = false;  // Reset each tick
  ec.input.reset();                   //Reset input for tick

  // raygui text size - 16 per default
  const auto scaleY = ec.display.screenSize.y / 1080.0F;
  const auto fontSize = fmaxf(13.0F, std::round(13.0F * scaleY));
  GuiSetStyle(DEFAULT, TEXT_SIZE, static_cast<int>(fontSize));
}

}  // namespace Editor
#endif  // RAYNODES_SRC_APPLICATION_EDITOR_EDITORUPDATE_H_