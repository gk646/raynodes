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

#include <raygui.h>

#include "ActionMenu.h"

#include "ToolTip.h"
#include "application/EditorContext.h"
#include "shared/rayutils.h"

namespace {
template <class T>
void InvokeAction(EditorContext& ec, typename ActionMenu<T>::ContextAction action) {
  if constexpr (std::is_same_v<T, Node>) {
    if (ec.logic.hoveredNode != nullptr) action(ec, *ec.logic.hoveredNode);
  } else if constexpr (std::is_same_v<T, NodeGroup>) {
    if (ec.logic.hoveredGroup != nullptr) action(ec, *ec.logic.hoveredGroup);
  } else if constexpr (std::is_same_v<T, Vector2>) {
    Vector2 vector2;
    action(ec, vector2);
  }
}
}  // namespace

template <class T>
void ActionMenu<T>::draw(EditorContext& ec, Vector2 pos) {
  const auto& font = ec.display.editorFont;
  const float fs = ec.display.fontSize;
  const auto mouse = ec.logic.mouse;
  const auto paddingX = 22.0F;
  const auto paddingY = 4.0F;
  const auto w = 200.0F;
  const auto entryHeight = fs + paddingY;

  const auto pressed = ec.input.isMBDown(MOUSE_BUTTON_LEFT) || ec.input.isMBPressed(MOUSE_BUTTON_LEFT);
  const auto released = ec.input.isMBReleased(MOUSE_BUTTON_LEFT);
  if (pressed) ec.input.consumeMouse();

  const auto background = ColorAlpha(UI::COLORS[UI_DARK], 0.97F);
  const auto highlight = ColorAlpha(UI::COLORS[UI_MEDIUM], 0.97F);

  // Draw Title
  DrawRectangleRec({pos.x, pos.y, w, entryHeight}, background);
  DrawTextEx(font, menuName, {pos.x + 2, pos.y}, fs + 1, 0.0F, UI::COLORS[UI_LIGHT]);
  pos.y += entryHeight;

  Rectangle quickActionEntry = {pos.x + 2, pos.y, 32, 32};

  // Draw Quick actions
  {
    DrawRectangleRec({pos.x, pos.y, w, 32}, background);
    GuiSetIconScale(2);
    for (const auto& [action, name, icon] : quickActions) {
      const auto hovered = CheckCollisionPointRec(mouse, quickActionEntry);
      const auto back = pressed ? UI::Darken(UI::COLORS[UI_MEDIUM], 25) : UI::COLORS[UI_MEDIUM];
      if (hovered) {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawRectangleRounded(quickActionEntry, 0.3F, 30, ColorAlpha(back, 0.4F));
        EndBlendMode();
        ToolTip::Set(name);
      }
      GuiDrawIcon(icon, quickActionEntry.x, quickActionEntry.y, 2, GRAY);
      if (hovered && released) {
        InvokeAction<T>(ec, action);
        isVisible = false;
      }
      quickActionEntry.x += 34;
    }
    GuiSetIconScale(1);
  }

  Rectangle entry = {pos.x, quickActionEntry.y + quickActionEntry.height, w, entryHeight};
  // Draw normal action list
  {
    int i = 0;
    for (const auto& [action, name, icon] : actions) {
      if (CheckCollisionPointRec(mouse, entry)) {
        DrawRectangleRounded(entry, 0.1F, 30, highlight);
        if (released) {
          InvokeAction<T>(ec, action);
          isVisible = false;
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
  }

  // Check out of bounds
  const auto totalHeight = static_cast<float>(actions.size() + 1) * entryHeight + 32;
  const auto bounds = Rectangle{entry.x, entry.y - totalHeight, entry.width, totalHeight};
  if (!CheckExtendedRec(mouse, bounds, UI::CONTEXT_MENU_THRESHOLD)) { isVisible = false; }
}

template void ActionMenu<Node>::draw(EditorContext& ec, Vector2 pos);
template void ActionMenu<NodeGroup>::draw(EditorContext& ec, Vector2 pos);
template void ActionMenu<Vector2>::draw(EditorContext& ec, Vector2 pos);