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

#include "ui/DropDown.h"

#include <raylib.h>

#include "../application/EditorContext.h"

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

  hoveredIndex = -1;
  if (isExpanded) {
    for (int i = 0; i < items.size(); i++) {
      float itemY = y + h * (i + 1);
      Rectangle itemBounds = {x, itemY, w, h};
      if (CheckCollisionPointRec(ec.logic.worldMouse, itemBounds)) {
        DrawRectangleRec(itemBounds, GRAY);
        hoveredIndex = i;
      } else {
        DrawRectangleRec(itemBounds, WHITE);
      }
      DrawTextEx(f, items[i].c_str(), {x + 5, itemY + 3}, fs, 1, BLACK);
    }
  }

  // Draw outline
  DrawRectangleLinesEx(bounds, 2, BLACK);
}

int DropDown::update(EditorContext& ec) {
  Rectangle bounds = {x, y, w, h};

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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