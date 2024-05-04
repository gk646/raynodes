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

#include "application/EditorContext.h"

void UserInterface::invokeFileMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;
  if (i == 1) ec.core.newFile(ec);
  if (i == 2) ec.core.open(ec);
  if (i == 3) ec.persist.saveToFile(ec, false);
  if (i == 4) ec.persist.saveToFile(ec, true);
  if (i == 5) ec.core.closeApplication = true;
}
void UserInterface::invokeEditMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 1) ec.core.undo(ec);
  if (i == 2) ec.core.redo(ec);
  if (i == 3) ec.core.cut(ec);
  if (i == 4) ec.core.copy(ec);
  if (i == 5) ec.core.paste(ec);
  if (i == 6) ec.core.erase(ec);
  if (i == 7) ec.core.selectAll(ec);
}
void UserInterface::invokeViewMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 1) ec.display.zoomIn();
  if (i == 2) ec.display.zoomOut();
  //TODO zoom to fit
  if (i == 4) ec.ui.showGrid = !ec.ui.showGrid;
}