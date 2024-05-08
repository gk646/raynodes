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
#include "application/EditorContext.h"

int UI::DrawButton(EditorContext& ec, Rectangle& r, const char* txt) {
  const auto bounds = ec.display.getFullyScaled(r);
  const auto res = GuiButton(bounds, txt);

  if (ec.ui.scaleDirection == HORIZONTAL) r.x += r.width;
  else r.y += r.height;

  if (res
      || (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)
          && CheckCollisionPointRec(ec.logic.mouse, bounds))) {
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