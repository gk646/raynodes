#ifndef RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_

#include "nodes/Nodes.h"

namespace Editor {
inline void PollControls(EditorContext& ec) {
  const auto mouse = ec.logic.mouse;
  const auto worldMouse = ec.logic.worldMouse;
  auto& dragStart = ec.logic.dragStart;
  auto& camera = ec.display.camera;
  auto& isDragging = ec.logic.isDraggingScreen;
  auto& selectRect = ec.logic.selectRect;
  auto& contextMenuPos = ec.logic.contextMenuPos;

  //Context menu
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    contextMenuPos = mouse;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    if (mouse.x == contextMenuPos.x && mouse.y == contextMenuPos.y) {
      ec.logic.showContextMenu = !ec.logic.showContextMenu;
      contextMenuPos.x -= 3;
      contextMenuPos.y -= 3;
      ec.logic.isDraggingScreen = false;
      ec.logic.isSelecting = false;
      return;
    }
  }

  //Panning
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ec.logic.isAnyNodeHovered) {
    ec.logic.isSelecting = false;
    ec.core.selectedNodes.clear();
    dragStart = worldMouse;
    isDragging = true;
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }
  if (isDragging) {
    camera.target.x -= (worldMouse.x - dragStart.x);
    camera.target.y -= (worldMouse.y - dragStart.y);
    dragStart = GetScreenToWorld2D(GetMousePosition(), camera);;
  }

  //Selecting
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !ec.logic.isAnyNodeHovered && !ec.logic.showContextMenu) {
    ec.logic.isSelecting = true;
    selectRect.x = worldMouse.x;
    selectRect.y = worldMouse.y;
    selectRect.width = 0;
    selectRect.height = 0;
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    ec.logic.isSelecting = false;
  }

  //Shortcuts
  if (IsKeyPressed(KEY_DELETE)) {
    for (auto pair : ec.core.selectedNodes) {
      //context.temporaryDelete(NodeID(pair.first));
    }
    ec.core.selectedNodes.clear();
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

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
