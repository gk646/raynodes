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

#include "PopupMenu.h"

#include "SimpleDropDown.h"
#include "TextField.h"
#include "application/EditorContext.h"
#include "shared/rayutils.h"

const char* PopupMenu::InputText(EditorContext& ec, const Rectangle& r, TextField& input, const ValidationFunc func,
                                 const char* txt) {
  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  const auto scaled = ec.display.getFullyScaled(r);
  DrawRectangleRec(scaled, UI::Darken(UI::COLORS[UI_MEDIUM]));
  DrawRectangleLinesEx(scaled, 1, UI::Darken(UI::COLORS[UI_DARK]));
  DrawCenteredText(f, txt, {scaled.x + scaled.width / 2.0F, scaled.y}, fs, 1.0F, UI::COLORS[UI_LIGHT]);

  const auto scaledPad = scaled.width * 0.1F;  // So we can pad in scaling

  input.bounds.width = scaled.width / 3.0F;
  input.bounds.height = 20;
  input.bounds.x = scaled.x + (scaled.width - input.bounds.width) / 2.0F;
  input.bounds.y = scaled.y + scaled.height / 5.0F;

  if (ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) {
    if (!CheckCollisionPointRec(ec.logic.mouse, scaled)) {
      input.buffer.clear();
      return UI::DUMMY_STRING;
    }
    input.onFocusGain(ec.logic.mouse);
  }
  input.update(ec, ec.logic.mouse);
  input.draw();

  const char* vRes = func(ec, input.buffer.c_str());
  const bool isValid = vRes == nullptr;

  const auto infoPos = Vector2{scaled.x + scaled.width / 2.0F, input.bounds.y + scaledPad};
  if (vRes != nullptr) {
    DrawCenteredText(f, vRes, infoPos, fs, 1.0F, RED);
  } else {
    DrawCenteredText(f, "Valid Name", infoPos, fs, 1.0F, UI::COLORS[UI_LIGHT]);
  }

  const auto buttonW = r.width / 5.0F;
  const auto buttonH = r.height / 8.0F;

  const auto buttonY = r.y + r.height * 0.7F;
  const auto leftButton = Rectangle{r.x + r.width * 0.1F, buttonY, buttonW, buttonH};
  const auto rightButton = Rectangle{r.x + r.width - (r.width * 0.1F + buttonW), buttonY, buttonW, buttonH};

  if (!isValid) GuiSetState(STATE_DISABLED);
  if (UI::DrawButton(ec, leftButton, "Confirm") || IsKeyPressed(KEY_ENTER)) {
    if (input.buffer.c_str() != nullptr && isValid) { return input.buffer.c_str(); }
  }
  GuiSetState(STATE_NORMAL);

  if (UI::DrawButton(ec, rightButton, "Cancel")) { return UI::DUMMY_STRING; }

  return nullptr;
}

const char* PopupMenu::InputTextEx(EditorContext& ec, Rectangle& r, TextField& input, ValidationFunc func,
                                   const char* header, CustomDraw customFunc) {
  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  const auto scaled = ec.display.getFullyScaled(r);
  DrawRectangleRec(scaled, UI::Darken(UI::COLORS[UI_MEDIUM]));
  DrawRectangleLinesEx(scaled, 1, UI::Darken(UI::COLORS[UI_DARK]));
  DrawCenteredText(f, header, {scaled.x + scaled.width / 2.0F, scaled.y}, fs, 1.0F, UI::COLORS[UI_LIGHT]);

  const auto scaledPad = scaled.width * 0.1F;  // So we can pad in scaling

  input.bounds.width = scaled.width / 3.0F;
  input.bounds.height = 20;
  input.bounds.x = scaled.x + (scaled.width - input.bounds.width) / 2.0F;
  input.bounds.y = scaled.y + scaled.height / 5.0F;

  input.update(ec, ec.logic.mouse);
  input.draw();

  const char* vRes = func(ec, input.buffer.c_str());
  bool isValid = vRes == nullptr;

  const auto infoPos = Vector2{scaled.x + scaled.width / 2.0F, input.bounds.y + scaledPad};
  if (vRes != nullptr) {
    DrawCenteredText(f, vRes, infoPos, fs, 1.0F, RED);
  } else {
    DrawCenteredText(f, "Valid Name", infoPos, fs, 1.0F, UI::COLORS[UI_LIGHT]);
  }

  const bool validCustom = customFunc(ec, r);

  // Update after so custom func has layer control
  if (ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) {
    if (!CheckCollisionPointRec(ec.logic.mouse, scaled)) {
      input.buffer.clear();
      return UI::DUMMY_STRING;
    }
    input.onFocusGain(ec.logic.mouse);
  }

  isValid = validCustom && isValid;

  const auto buttonW = r.width / 5.0F;
  const auto buttonH = r.height / 8.0F;

  const auto buttonY = r.y + r.height * 0.7F;
  const auto leftButton = Rectangle{r.x + r.width * 0.1F, buttonY, buttonW, buttonH};
  const auto rightButton = Rectangle{r.x + r.width - (r.width * 0.1F + buttonW), buttonY, buttonW, buttonH};

  if (!isValid) GuiSetState(STATE_DISABLED);
  if (UI::DrawButton(ec, leftButton, "Confirm") || IsKeyPressed(KEY_ENTER)) {
    if (input.buffer.c_str() != nullptr && isValid) { return input.buffer.c_str(); }
  }
  GuiSetState(STATE_NORMAL);

  if (UI::DrawButton(ec, rightButton, "Cancel")) { return UI::DUMMY_STRING; }

  return nullptr;
}