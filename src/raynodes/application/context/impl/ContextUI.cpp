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
#include <cxutil/cxstring.h>
#include "application/EditorContext.h"

#include "ui/windows/HelpMenu.h"
#include "ui/windows/NodeCreator.h"
#include "ui/windows/SettingsMenu.h"

UI::UI() {
  const auto bounds = GetCenteredWindowBounds<true>();
  windows.push_back(new HelpMenu(bounds, "Help"));
  windows.push_back(new SettingsMenu(bounds, "Settings"));
  windows.push_back(new NodeCreator(bounds, "Node Creator"));
}
bool UI::loadUI(EditorContext& ec) {
  getWindow<NodeCreator>(NODE_CREATOR)->searchField.font = &ec.display.editorFont;
  getWindow<NodeCreator>(NODE_CREATOR)->componentSearchField.font = &ec.display.editorFont;
  getWindow<NodeCreator>(NODE_CREATOR)->popupField.font = &ec.display.editorFont;
  return true;
}
int UI::DrawListMenu(EditorContext& ec, bool& open, const char* title, const char* listText, int& active) {
  if (ec.input.isKeyPressed(KEY_ESCAPE)) open = false;
  if (!open) return -1;
  const auto windowRect = GetCenteredWindowBounds();
  if (DrawWindow(ec, windowRect, title)) { open = false; }
  constexpr float pad = 25.0F;
  constexpr float listW = 150.0F;
  int scroll = 0;

  const Rectangle listBounds = {windowRect.x, windowRect.y + pad, listW, windowRect.height - pad};
  GuiListView(ec.display.getFullyScaled(listBounds), listText, &scroll, &active);
  return active;
}
int UI::DrawButton(EditorContext& ec, const Rectangle& r, const char* txt) {
  const auto bounds = ec.display.getFullyScaled(r);
  const auto res = GuiButton(bounds, txt);

  if (res || (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(ec.logic.mouse, bounds))) {
    ec.input.consumeMouse();  // Consume mouse so click doesnt propagate
  }
  return res;
}
int UI::DrawButton(EditorContext& ec, const Vector2& pos, float w, float h, const char* txt) {
  const auto bounds = ec.display.getFullyScaled({pos.x, pos.y, w, h});
  const auto res = GuiButton(bounds, txt);

  if (res || (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(ec.logic.mouse, bounds))) {
    ec.input.consumeMouse();  // Consume mouse so click doesnt propagate
  }
  return res;
}
int UI::DrawWindow(EditorContext& ec, const Rectangle& r, const char* txt) {
  const auto bounds = ec.display.getFullyScaled(r);
  const auto res = GuiWindowBox(bounds, txt);
  if (res || CheckCollisionPointRec(ec.logic.mouse, bounds)) {
    ec.input.consumeMouse();     // Consume mouse so click doesnt propagate
    ec.input.consumeKeyboard();  // Consume mouse so click doesnt propagate
  }
  return res;
}
void UI::DrawText(EditorContext& ec, Vector2 pos, const char* txt, Color c, bool hasIcons) {
  const auto& font = ec.display.editorFont;
  const auto& fs = ec.display.fontSize;
  pos = ec.display.getFullyScaled(pos);

  // Quick and dirty way of drawing icons dynamically
  if (hasIcons) {
    const char* start = txt;
    const char* p = txt;
    while (*p != '\0') {
      if (*p == '#') {
        if (p != start) {
          ec.string.formatText("%s", start);
          ec.string.buffer[p - start] = '\0';
          DrawTextEx(font, ec.string.buffer, pos, fs, 1.0F, c);
          pos.x += MeasureTextEx(font, ec.string.buffer, fs, 1.0F).x;
        }
        p++;
        int iconID = cxstructs::str_parse_int(p);
        GuiDrawIcon(iconID, pos.x, pos.y, 1, COLORS[UI_DARK]);
        p += 4;
        start = p;
        pos.x += 16;
      } else {
        p++;
      }
    }
    if (*start) { DrawTextEx(font, start, pos, fs, 1.0F, c); }
  } else {
    DrawTextEx(font, txt, pos, fs, 1.0F, c);
  }
}
void UI::DrawRect(EditorContext& ec, Rectangle rec, int borderWidth, Color borderColor, Color color) {
  // Copied from raygui.h
  rec = ec.display.getFullyScaled(rec);
  if (color.a > 0) {
    // Draw rectangle filled with color
    DrawRectangle((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, color);
  }

  if (borderWidth > 0) {
    // Draw rectangle border lines with color
    DrawRectangle((int)rec.x, (int)rec.y, (int)rec.width, borderWidth, borderColor);
    DrawRectangle((int)rec.x, (int)rec.y + borderWidth, borderWidth, (int)rec.height - 2 * borderWidth, borderColor);
    DrawRectangle((int)rec.x + (int)rec.width - borderWidth, (int)rec.y + borderWidth, borderWidth,
                  (int)rec.height - 2 * borderWidth, borderColor);
    DrawRectangle((int)rec.x, (int)rec.y + (int)rec.height - borderWidth, (int)rec.width, borderWidth, borderColor);
  }
}
Rectangle UI::GetSubRect(const Rectangle& r) {
  float insetX = r.width / 5.0f;
  float insetY = r.height / 5.0f;

  float newWidth = r.width - 2 * insetX;
  float newHeight = r.height - 2 * insetY;

  float newX = r.x + insetX;
  float newY = r.y + insetY;

  return {newX, newY, newWidth, newHeight};
}
void UI::invokeFileMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;
  if (i == 1) ec.core.newFile(ec);
  if (i == 2) ec.core.open(ec);
  if (i == 3) ec.persist.saveToFile(ec, false);
  if (i == 4) ec.persist.saveToFile(ec, true);
  if (i == 5) ec.core.closeApplication = true;
}
void UI::invokeEditMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 1) ec.core.undo(ec);
  if (i == 2) ec.core.redo(ec);
  if (i == 3) ec.core.cut(ec);
  if (i == 4) ec.core.copy(ec);
  if (i == 5) ec.core.paste(ec);
  if (i == 6) ec.core.erase(ec);
  if (i == 7) ec.core.selectAll(ec);
}
void UI::invokeViewMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 1) ec.display.zoomIn();
  if (i == 2) ec.display.zoomOut();
  //TODO zoom to fit
  if (i == 4) ec.ui.showGrid = !ec.ui.showGrid;
}
void UI::invokeHelpMenu(EditorContext& ec, int i) {}
void UI::invokeSettingsMenu(EditorContext& ec, int i) {}