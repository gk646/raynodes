#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_

#include "nodes/Nodes.h"

namespace Editor {
void DrawGrid(EditorContext& ec) {
  constexpr Color color{58, 68, 102, 255};
  float baseGridSpacing = ec.display.gridSpacing;
  const auto& camera = ec.display.camera;
  // Calculate the visible world area
  Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, camera);

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
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ec.core.createNode(NodeType(i), GetScreenToWorld2D(pos, ec.display.camera));
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
}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_
