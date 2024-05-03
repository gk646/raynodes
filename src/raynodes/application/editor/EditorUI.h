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

#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_

namespace {
void CreateNewNode(EditorContext& ec, Vector2 pos, const char* name) {
  const auto newN = ec.core.createNode(ec, name, GetScreenToWorld2D(pos, ec.display.camera));
  if (!newN) return;
  const auto action = new NodeCreateAction(2);
  action->createdNodes.push_back(newN);
  ec.core.addEditorAction(action);
}
}  // namespace

namespace Editor {
inline void DrawGrid(const EditorContext& ec) {
  if (!ec.ui.showGrid) return;
  constexpr Color color{58, 68, 102, 255};
  const float baseGridSpacing = ec.display.gridSpacing;
  const auto& camera = ec.display.camera;
  // Calculate the visible world area
  Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D({ec.display.screenSize.x, ec.display.screenSize.y}, camera);

  // Calculate the starting points for drawing grid lines, adjusted for zoom
  float startX = floor(topLeft.x / baseGridSpacing) * baseGridSpacing;
  float startY = floor(topLeft.y / baseGridSpacing) * baseGridSpacing;

  // Calculate the end points for drawing grid lines, adjusted for zoom
  float endX = ceil(bottomRight.x / baseGridSpacing) * baseGridSpacing;
  float endY = ceil(bottomRight.y / baseGridSpacing) * baseGridSpacing;

  // Draw vertical grid lines
  for (float x = startX; x <= endX; x += baseGridSpacing) {
    // Convert world coordinates to screen coordinates for drawing
    Vector2 start = GetWorldToScreen2D({x, topLeft.y}, camera);
    Vector2 end = GetWorldToScreen2D({x, bottomRight.y}, camera);
    DrawLineV(start, end, color);
  }

  // Draw horizontal grid lines
  for (float y = startY; y <= endY; y += baseGridSpacing) {
    // Convert world coordinates to screen coordinates for drawing
    Vector2 start = GetWorldToScreen2D({topLeft.x, y}, camera);
    Vector2 end = GetWorldToScreen2D({bottomRight.x, y}, camera);
    DrawLineV(start, end, color);
  }
}
inline void DrawContextMenu(EditorContext& ec) {
  if (!ec.logic.showContextMenu) return;
  constexpr float menuWidth = 170.0F;
  auto& categories = ec.ui.contextMenu.categories;
  const auto& font = ec.display.editorFont;
  const auto mouse = ec.logic.mouse;
  const auto fs = ec.display.fontSize;
  const auto pos = ec.logic.contextMenuPos;
  const auto hightLightColor = ColorAlpha(WHITE, 0.35);
  const auto padding = fs * 0.3F;
  const auto height = (fs + padding) * static_cast<float>(categories.size());
  const Rectangle menuRect = {pos.x, pos.y, menuWidth, height};
  Vector2 drawPos{pos.x, pos.y};

  // Draw main menu background
  DrawRectanglePro(menuRect, {0, 0}, 0, ColorAlpha(BLACK, 0.5));

  const ContextMenuCategory* hoveredCategory = nullptr;

  for (auto& category : categories) {
    const Rectangle textRect = {drawPos.x, drawPos.y, menuWidth, fs + padding};
    float categoryHeight = static_cast<float>(category.nodes.size()) * (fs + padding);
    Rectangle categoryRect = {drawPos.x + menuWidth, drawPos.y, menuWidth, categoryHeight};

    // Detect if mouse is over this category
    if (CheckCollisionPointRec(mouse, textRect)
        || (CheckCollisionPointRec(mouse, categoryRect) && category.isOpen)) {
      hoveredCategory = &category;  // Update the currently hovered category
    }

    category.isOpen = hoveredCategory == &category;

    // Highlight the category if it's open
    if (category.isOpen) {
      DrawRectangleRec(textRect, hightLightColor);
    }

    DrawTextEx(font, category.name, {drawPos.x + padding, drawPos.y + padding}, fs, 1.0F, WHITE);

    // Draw the category's nodes if it's open
    if (category.isOpen && !category.nodes.empty()) {
      Vector2 drawPosC = {drawPos.x + menuWidth, drawPos.y};
      DrawRectangleRec(categoryRect, ColorAlpha(BLACK, 0.6));
      for (const auto& name : category.nodes) {
        const Rectangle nodeTextRect = {drawPosC.x, drawPosC.y, menuWidth, fs + padding};
        if (CheckCollisionPointRec(mouse, nodeTextRect)) {
          DrawRectangleRec(nodeTextRect, hightLightColor);
          if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            CreateNewNode(ec, pos, name);
            hoveredCategory = nullptr;
            break;
          }
        }
        DrawTextEx(font, name, {drawPosC.x + padding, drawPosC.y + padding}, fs, 1.0F, WHITE);
        drawPosC.y += fs + padding;
      }
    }

    drawPos.y += fs + padding;
  }

  // If no category is hovered, close all and hide the context menu
  if (!hoveredCategory) {
    for (auto& category : categories) {
      category.isOpen = false;
    }
    ec.logic.showContextMenu = false;
  }
}
inline void DrawActions(EditorContext& ec) {
  constexpr int visibleActions = 10;  // Number of actions displayed at a time

  // Cache
  const auto& font = ec.display.editorFont;
  const auto width = ec.display.getSpace(0.17);
  const auto padding = width * 0.03F;
  const auto height = visibleActions * ec.display.getSpace(0.022);
  const auto [x, y] = ec.display.getAnchor(RIGHT_TOP, 0.03, width, height);
  const auto fs = ec.display.fontSize;
  const auto currActionIdx = ec.core.currentActionIndex;

  auto rect = Rectangle{x, y, width, height};

  DrawRectangleRec(rect, ColorAlpha(DARKGRAY, 0.7F));
  DrawRectangleLinesEx(rect, padding / 4.0F, ColorAlpha(GRAY, 0.7F));

  // Adjust the start index to ensure the current action is always on screen
  const int totalActions = static_cast<int>(ec.core.actionQueue.size());
  const int start = std::max(0, std::min(currActionIdx, totalActions - visibleActions));
  const int end = std::min(start + visibleActions - 1, totalActions - 1);

  rect.y += padding;

  //Coloring scheme is inspired by paint.net

  for (int i = start; i <= end; ++i) {
    const auto action = ec.core.actionQueue[i];
    const bool isCurrentAction = i == currActionIdx;
    const bool isCurrentOrBelow = i >= currActionIdx;
    const bool isCurrentOrAbove = i <= currActionIdx;

    // All text is white expect past actions
    const Color textColor = isCurrentOrAbove ? RAYWHITE : BLACK;

    // Only current events is blue, past ones gray
    const Color highLightColor = isCurrentAction ? BLUE : GRAY;

    // Draw background highlight for the current action
    if (isCurrentOrBelow) {
      DrawRectangleRec({rect.x + padding, rect.y, width - 2 * padding, fs + padding},
                       ColorAlpha(highLightColor, 0.4F));
    }

    // Draw action text
    DrawTextEx(font, action->toString(), {rect.x + padding, rect.y + padding / 2}, fs, 1.0F, textColor);

    // Move to the next action's position, ensuring space for text and padding
    rect.y += fs + padding;
  }
}
inline void DrawTopBar(EditorContext& ec) {
  constexpr auto dropDown = [](EditorContext& ec, const Rectangle& r, const char* text, bool& state) {
    // Always reset to 0 cause its a dropdown
    int active = 0;
    // We use extra getter to properly scale it
    if (GuiDropdownBox(ec.display.getSmartScaled(r), text, &active, state)) {
      state = !state;
      ec.input.consumeMouse();
      ec.logic.isDraggingScreen = false;
      return active;
    }
    return -1;
  };

  const auto height = ec.ui.topBarHeight;
  int res = -1;
  res = dropDown(ec, {20, 5, 100, height}, ec.ui.fileMenuText, ec.ui.fileMenuState);
  ec.ui.invokeFileMenu(ec, res);

  res = dropDown(ec, {120, 5, 100, height}, ec.ui.editMenuText, ec.ui.editMenuState);
  ec.ui.invokeEditMenu(ec, res);

  res = dropDown(ec, {220, 5, 100, height}, ec.ui.viewMenuText, ec.ui.viewMenuState);
  ec.ui.invokeViewMenu(ec, res);
}
inline void DrawStatusBar(EditorContext& ec) {
  float x = 0;
  constexpr float height = 18.0F;
  constexpr float y = 1080 - height;

  constexpr auto statusBar = [](EditorContext& ec, float& x, float y, float w, float h, const char* text) {
    GuiStatusBar(ec.display.getFullyScaled({x, y, w, h}), text);
    x += w;
  };

  statusBar(ec, x, y, 1200.0F, height, nullptr);

  // Show mouse position
  auto* text = String::FormatText("#021# %d/%d", (int)ec.logic.worldMouse.x, (int)ec.logic.worldMouse.y);
  statusBar(ec, x, y, 150.0F, height, text);

  const auto lastWidth = 1920.0F - x;
  statusBar(ec, x, y, lastWidth, height, nullptr);

  // Draw the Zoom slider
  const auto bounds = ec.display.getFullyScaled({x - lastWidth + 25, y, lastWidth - 50, height});
  GuiSliderBar(bounds, nullptr, nullptr, &ec.display.camera.zoom, 0.1F, 3.0F);

  // Workaround - raygui sadly doesnt properly handle return value here
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(ec.logic.mouse, bounds)) {
    ec.input.consumeMouse();
  }

  // Draw zoom labels
  if (GuiButton(ec.display.getFullyScaled({x - lastWidth, y, 25, 18}), "#221#")) {
    ec.display.zoomOut();
  }
  if(GuiButton(ec.display.getFullyScaled({x - 25, y, 25, 18}), "#220#")) {
    ec.display.zoomIn();
  }
}
}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_