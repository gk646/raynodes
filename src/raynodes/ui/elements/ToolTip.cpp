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

#include "ToolTip.h"

#include "application/EditorContext.h"

void ToolTip::Draw(EditorContext& ec) {
  if (toolTip) showCounter++;
  else showCounter = 0;
  if (!toolTip || showCounter < showDely) return;
  const auto mouse = ec.logic.mouse;
  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  const auto width = MeasureTextEx(f, toolTip, fs, 0.5F).x;

  DrawRectangleRounded({mouse.x, mouse.y + 20, width, 20}, 0.5F, 25, UI::COLORS[UI_MEDIUM]);
  DrawTextEx(f, toolTip, {mouse.x, mouse.y + 21}, fs, 0.5F, UI::COLORS[UI_LIGHT]);
}