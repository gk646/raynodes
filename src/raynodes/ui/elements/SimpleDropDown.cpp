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

#include "ui/elements/SimpleDropDown.h"

#include <raylib.h>
#include <raygui.h>

#include "ui/elements/TextField.h"
#include "application/EditorContext.h"

void SimpleDropDown::draw(EditorContext& ec, float dx, float dy) {
  x = dx, y = dy;

  const Font& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  Rectangle bounds = {x, y, w, h * (isExpanded ? static_cast<float>(items.size()) + 1.0F : 1.0F)};

  DrawRectangleRec(bounds, LIGHTGRAY);  // Draw outer bounds

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
int SimpleDropDown::update(EditorContext& ec) {
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

void SimpleDropDown::DrawSearchDropdown(EditorContext& ec, Vector2 pos, TextField& search, const SortVector& items) {
  bool& open = search.isFocused;
  constexpr float padding = 4;
  const auto& font = ec.display.editorFont;
  const float fs = ec.display.fontSize;
  const float entryHeight = fs + padding;
  const auto mouse = ec.logic.mouse;

  float maxWidth = 0;
  for (const auto item : items) {
    const float itemWidth = MeasureTextEx(font, item, fs, 0.5F).x + padding * 2;  // Additional padding
    if (itemWidth > maxWidth) maxWidth = itemWidth;
  }

  pos.x = pos.x - maxWidth / 2;
  pos.y = pos.y - entryHeight / 2;

  pos.y += entryHeight;
  const Rectangle dropdownRect = {pos.x, pos.y, maxWidth, entryHeight};

  // Draw the text field
  bool pressed = ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);
  const auto [x, y] = ec.display.getFullyScaled(Vector2{150, 20});

  search.bounds = dropdownRect;
  search.bounds.width = x;
  search.bounds.height = y;

  search.draw("Type...");
  search.update(ec, ec.logic.mouse);

  float totalHeight = items.size() * entryHeight;
  Rectangle scrollPanelRec = {pos.x, pos.y + entryHeight, maxWidth, std::min(200.0f, totalHeight)};

  if (open) {
    Rectangle contentRec = {0, 0, maxWidth - 20, totalHeight};  // -20 for scrollbar width
    static Vector2 scroll = {0, 0};
    Rectangle view;

    GuiScrollPanel(scrollPanelRec, nullptr, contentRec, &scroll, &view);

    BeginScissorMode(view.x, view.y, view.width, view.height);
    for (int i = 0; i < items.size(); ++i) {
      Rectangle itemRec = {pos.x, pos.y + entryHeight + i * entryHeight + scroll.y, maxWidth - 20, entryHeight};
      bool isHovered = CheckCollisionPointRec(mouse, itemRec);
      if (pressed) ec.input.consumeMouse();
      if (isHovered && pressed && CheckCollisionPointRec(mouse, scrollPanelRec)) {
        search.buffer = items[i];
        open = false;
        EndScissorMode();
        return;
      }
      DrawRectangleRec(itemRec, isHovered ? UI::COLORS[UI_MEDIUM] : UI::COLORS[UI_DARK]);
      DrawTextEx(font, items[i], {itemRec.x + padding, itemRec.y + padding / 2.0F}, fs, 1.0F, UI::COLORS[UI_LIGHT]);
    }
    EndScissorMode();
    if (pressed && CheckCollisionPointRec(mouse, scrollPanelRec)) pressed = false;
  }
  if (pressed) { search.onFocusGain(ec.logic.mouse); }
}