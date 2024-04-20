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
//

#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_

namespace Editor {
inline void DrawGrid(const EditorContext& ec) {
  constexpr Color color{58, 68, 102, 255};
  const float baseGridSpacing = ec.display.gridSpacing;
  const auto& camera = ec.display.camera;
  // Calculate the visible world area
  Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
  Vector2 bottomRight =
      GetScreenToWorld2D({ec.display.screenSize.x, ec.display.screenSize.y}, camera);

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
  //Getters
  const auto fs = ec.display.fontSize;
  const auto pos = ec.logic.contextMenuPos;
  const auto& font = ec.display.editorFont;

  //Drawing
  const auto padding = fs * 0.3F;
  const auto height = (fs + padding) * (float)NodeType::TOTAL_SIZE;
  const auto rect = Rectangle{pos.x, pos.y, 170, height};
  DrawRectanglePro(rect, {0, 0}, 0, ColorAlpha(BLACK, 0.6));
  auto drawPos = Vector2{pos.x + padding, pos.y + padding};

  //Loop
  for (int i = 0; i < (int)NodeType::TOTAL_SIZE; i++) {
    const auto text = NodeToString(NodeType(i));
    auto textDims = MeasureTextEx(font, text, fs, 1.0F);
    const auto textRect = Rectangle{drawPos.x, drawPos.y, textDims.x, textDims.y};

    //Check hover and click state
    if (CheckCollisionPointRec(GetMousePosition(), textRect)) {
      DrawRectanglePro(textRect, {0, 0}, 0, ColorAlpha(WHITE, 0.6F));
      if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        auto action = new NodeCreateAction(2);
        auto newN = ec.core.createNode(ec, NodeType(i), GetScreenToWorld2D(pos, ec.display.camera));
        action->createdNodes.push_back(newN);
        ec.core.addEditorAction(action);
        ec.logic.showContextMenu = false;
      }
    }

    DrawTextEx(font, text, drawPos, fs, 1.0F, WHITE);
    drawPos.y += fs + padding;
  }

  if (!CheckCollisionPointRec(GetMousePosition(), rect)) {
    ec.logic.showContextMenu = false;
  }
}

inline void DrawActions(EditorContext& ec) {
  constexpr int visibleActions = 10;  // Number of actions you want to display at a time
  const auto width = ec.display.getSpace(0.17);
  const auto padding = width * 0.03F;
  const auto height = visibleActions * ec.display.getSpace(0.022);
  const auto [x, y] = ec.display.getAnchor(RIGHT_TOP, 0.03, width, height);
  const auto fs = ec.display.fontSize;
  const auto currActionIdx = ec.core.currentActionIndex;
  const auto& font = ec.display.editorFont;

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
    DrawTextEx(font, action->toString(), {rect.x + padding, rect.y + padding / 2}, fs, 1.0F,
               textColor);

    // Move to the next action's position, ensuring space for text and padding
    rect.y += fs + padding;
  }
}
}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_