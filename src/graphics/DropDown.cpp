#include "DropDown.h"

#include <raylib.h>

#include "application/EditorContext.h"

void DropDown::draw(EditorContext& ec, float dx, float dy) {
  x = dx;
  y = dy;
  const Font& f = ec.display.editorFont;
  const int fs = ec.display.fontSize;
  Rectangle bounds = {x, y, w, h};

  bounds.height = h * (isExpanded ? items.size() + 1 : 1);

  DrawRectangleRec(bounds, LIGHTGRAY);

  Vector2 textPos = {x + 5, y + 3};
  DrawTextEx(f, items[selectedIndex].c_str(), textPos, fs, 1, BLACK);

  if (isExpanded) {
    for (int i = 0; i < items.size(); i++) {
      float itemY = y + h * (i + 1);
      Rectangle itemBounds = {x, itemY, w, h};
      DrawRectangleRec(itemBounds, CheckCollisionPointRec(ec.logic.worldMouse, itemBounds) ? GRAY : WHITE);
      DrawTextEx(f, items[i].c_str(), {x + 5, itemY +3}, fs, 1, BLACK);
    }
  }

  // Draw outline
  DrawRectangleLinesEx(bounds, 2, BLACK);
}

int DropDown::update(EditorContext& ec) {
  Rectangle bounds = {x, y, w, h};

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (CheckCollisionPointRec(ec.logic.worldMouse, bounds)) {
      isExpanded = !isExpanded;
    } else if (isExpanded) {
      for (int i = 0; i < items.size(); i++) {
        float itemY = y + h * (i + 1);
        Rectangle itemBounds = {x, itemY, w, h};
        if (CheckCollisionPointRec(ec.logic.worldMouse, itemBounds)) {
          selectedIndex = i;
          isExpanded = false;
          break;
        }
      }
    }
  }

  return selectedIndex;
}