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

#include "TextPopup.h"

#include "TextField.h"
#include "application/EditorContext.h"
#include "shared/rayutils.h"

const char* TextPopup::Draw(EditorContext& ec, Rectangle& r, TextField& input, ValidationFunc func,
                            const char* txt) {
  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  r = ec.display.getFullyScaled(r);
  DrawRectangleRec(r, UI::Darken(UI::COLORS[UI_MEDIUM]));
  DrawRectangleLinesEx(r, 1, UI::Darken(UI::COLORS[UI_DARK]));
  DrawCenteredText(f, txt, {r.x + r.width / 2.0F, r.y}, fs, 1.0F, UI::COLORS[UI_LIGHT]);

  const auto scaledPad = r.width * 0.1F;  // So we can pad in scaling

  input.bounds.width = r.width / 3.0F;
  input.bounds.height = 20;
  input.bounds.x = r.x + (r.width - input.bounds.width) / 2.0F;
  input.bounds.y = r.y + r.height / 5.0F;

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) { input.onFocusGain(ec.logic.mouse); }
  input.update(ec, ec.logic.mouse);
  input.draw();

  const char* vRes = func(ec, input.buffer.c_str());
  const bool isValid = vRes == nullptr;

  const auto infoPos = Vector2{r.x + r.width / 2.0F, input.bounds.y + scaledPad};
  if (vRes != nullptr) {
    DrawCenteredText(f, vRes, infoPos, fs, 1.0F, RED);
  } else {
    DrawCenteredText(f, "Valid Name", infoPos, fs, 1.0F, UI::COLORS[UI_LIGHT]);
  }

  const auto buttonW = r.width / 5.0F;
  const auto buttonH = r.height / 8.0F;

  const auto buttonY = r.y + r.height * 0.7F;
  const auto leftButton = Rectangle{r.x + scaledPad, buttonY, buttonW, buttonH};
  const auto rightButton = Rectangle{r.x + r.width - (scaledPad + buttonW), buttonY, buttonW, buttonH};

  if (!isValid) GuiSetState(STATE_DISABLED);
  if (UI::DrawButton(ec, leftButton, "Confirm") || IsKeyPressed(KEY_ENTER)) {
    if (input.buffer.c_str() != nullptr && isValid) { return input.buffer.c_str(); }
  }
  GuiSetState(STATE_NORMAL);

  if (UI::DrawButton(ec, rightButton, "Cancel")) { return UI::DUMMY_STRING; }

  return nullptr;
}