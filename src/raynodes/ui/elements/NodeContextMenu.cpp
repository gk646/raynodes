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

#include <raylib.h>
#include <raygui.h>

#include "NodeContextMenu.h"

#include "ToolTip.h"
#include "application/EditorContext.h"
#include "shared/rayutils.h"

void NodeContextMenu::draw(EditorContext& ec, const Vector2& pos) {
  const auto& font = ec.display.editorFont;
  const float fs = ec.display.fontSize;
  const auto mouse = ec.logic.mouse;
  const auto [paddingX, paddingY, w, h] = Rectangle{22, 4, 200, fs + 4};
  const auto entryHeight = fs + paddingY;

  const auto pressed =
      ec.input.isMouseButtonDown(MOUSE_BUTTON_LEFT) || ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);
  const auto released = ec.input.isMouseButtonReleased(MOUSE_BUTTON_LEFT);
  if (pressed) ec.input.consumeMouse();

  const auto background = ColorAlpha(UI::COLORS[UI_DARK], 0.97F);
  const auto highlight = ColorAlpha(UI::COLORS[UI_MEDIUM], 0.97F);

  // Draw First Row - Quick actions
  DrawRectangleRec({pos.x, pos.y, w, 32}, background);

  Rectangle entry = {pos.x, pos.y, 32, 32};
  entry.x += 2;
  GuiSetIconScale(2);
  for (const auto& [action, name, icon] : quickActions) {
    const auto hovered = CheckCollisionPointRec(mouse, entry);
    const auto back = pressed ? UI::Darken(UI::COLORS[UI_MEDIUM], 25) : UI::COLORS[UI_MEDIUM];
    if (hovered) {
      BeginBlendMode(BLEND_ADDITIVE);
      DrawRectangleRounded(entry, 0.3F, 30, ColorAlpha(back, 0.4F));
      EndBlendMode();
      ToolTip::Set(name);
    }
    GuiDrawIcon(icon, entry.x, entry.y, 2, GRAY);
    if (hovered && released && ec.logic.hoveredNode) {
      action(ec, *ec.logic.hoveredNode);
      ec.logic.showNodeContextMenu = false;
    }
    entry.x += 34;
  }

  GuiSetIconScale(1);

  entry.x = pos.x;
  entry.y += entry.height;
  entry.width = w;
  entry.height = h;

  int i = 0;
  for (const auto& [action, name, icon] : actions) {
    if (CheckCollisionPointRec(mouse, entry)) {
      DrawRectangleRounded(entry, 0.1F, 30, highlight);
      if (released && ec.logic.hoveredNode != nullptr) {
        action(ec, *ec.logic.hoveredNode);
        ec.logic.showNodeContextMenu = false;
      }
    } else {
      DrawRectangleRec(entry, background);
    }
    const auto textPos = Vector2{entry.x + paddingX, entry.y + paddingY / 2.0F};
    constexpr auto color = UI::COLORS[UI_LIGHT];
    GuiDrawIcon(icon, static_cast<int>(entry.x + 2), static_cast<int>(textPos.y), 1, color);
    DrawTextEx(font, name, textPos, fs, 0.5F, color);
    entry.y += entryHeight;
    i++;
  }

  const auto totalHeight = static_cast<float>(actions.size()) * entryHeight + h * 1.5F;
  if (!CheckExtendedRec(mouse, {entry.x, entry.y - totalHeight, entry.width, totalHeight},
                        UI::CONTEXT_MENU_THRESHOLD)) {
    ec.logic.showNodeContextMenu = false;
  }
}