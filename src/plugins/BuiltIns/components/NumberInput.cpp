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

#include "NumberInput.h"

#include <cxutil/cxmath.h>
#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"
#include "shared/rayutils.h"

void NumberInput::draw(EditorContext& ec, Node& parent) {
  //Cache
  const auto bounds = getBounds();
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  //Safety measure
  cursorPos = cxstructs::clamp(static_cast<int>(cursorPos), 0, static_cast<int>(buffer.size()));

  //Measure cursor position
  const auto beforeWidth = MeasureTextUpTo(buffer.data(), cursorPos, font, fs, 0.5F);

  //Main field
  DrawRectangleRec(bounds, isFocused ? DARKGRAY : GRAY);
  //Draw the text
  DrawTextEx(font, buffer.c_str(), {bounds.x + 3, bounds.y}, fs, 0.5F, WHITE);
  //Draw cursor
  if (showCursor) {
    DrawTextEx(font, "|", {bounds.x + beforeWidth, bounds.y}, fs, 0.5F, WHITE);
  }
}

void NumberInput::update(EditorContext& ec, Node& parent) {
  auto inText = inputs[0].getData<PinType::STRING>();
  if (inText != nullptr) buffer = inText;
  outputs[0].setData<PinType::STRING>(buffer.c_str());

  if (!isFocused) return;

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    cursorPos = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);
  }

  int key = GetCharPressed();
  while (key > 0) {
    if ((key >= 48 && key <= 57) || key == 46 /* dot*/) {
      buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
      cursorPos++;
    }
    key = GetCharPressed();  // Check next character in the queue
  }

  if (cursorPos > 0 && (ec.input.isKeyPressed(KEY_BACKSPACE) || ec.input.isKeyPressedRepeat(KEY_BACKSPACE))) {
    buffer.erase(buffer.begin() + cursorPos - 1);
    cursorPos--;
  }

  if (cursorPos < buffer.size()
      && (ec.input.isKeyPressed(KEY_DELETE) || ec.input.isKeyPressedRepeat(KEY_DELETE))) {
    buffer.erase(buffer.begin() + cursorPos);
  }

  if (cursorPos < buffer.length()
      && (ec.input.isKeyPressed(KEY_RIGHT) || ec.input.isKeyPressedRepeat(KEY_RIGHT))) {
    cursorPos++;
  }

  if (cursorPos > 0 && (ec.input.isKeyPressed(KEY_LEFT) || ec.input.isKeyPressedRepeat(KEY_LEFT))) {
    cursorPos--;
  }

  // Handle blinking cursor
  blinkCounter++;
  if (blinkCounter >= BLINK_DELAY) {
    showCursor = !showCursor;
    blinkCounter = 0;
  }
}

void NumberInput::onFocusGain(EditorContext& ec) {
  //Position the cursor correct inside the text
  cursorPos = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);

  blinkCounter = 0;
  showCursor = true;
  if (currAction) {
    delete currAction;
    currAction = nullptr;
  }
  currAction = new TextAction(buffer, buffer);
}

void NumberInput::onFocusLoss(EditorContext& ec) {
  showCursor = false;
  if (currAction) {
    //TRANSFER OWNERSHIP
    if (currAction->beforeState == buffer) {
      delete currAction;
      currAction = nullptr;
      return;
    }
    currAction->setAfter(buffer);
    ec.core.addEditorAction(currAction);
    currAction = nullptr;
  }
}

void NumberInput::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label
  addPinInput(PinType::INTEGER);
  addPinInput(PinType::FLOAT);

  addPinOutput(PinType::INTEGER);
  addPinOutput(PinType::FLOAT);
}

void NumberInput::save(FILE* file) {
  cxstructs::io_save(file, buffer.c_str());
}

void NumberInput::load(FILE* file) {
  cxstructs::io_load(file, buffer);
}

uint16_t NumberInput::getIndexFromPos(const Font& font, const float fs, const Vector2 mouse) const {
  const float relX = mouse.x - x;
  float accu = 0;
  char buff[2] = {};
  for (int i = 0; i < buffer.size(); ++i) {
    buff[0] = buffer[i];
    if (buff[0] == '\n') {
      accu = 0;
      continue;
    }
    if (buff) accu += MeasureTextEx(font, buff, fs, 0.5F).x;
    if (accu >= relX) {
      return static_cast<uint16_t>(i);
    }
  }
  if (relX > accu) return static_cast<uint16_t>(buffer.size());
  return 0;
}