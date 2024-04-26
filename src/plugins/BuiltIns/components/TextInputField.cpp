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

#include "TextInputField.h"

#include <cxutil/cxmath.h>
#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"
#include "shared/rayutils.h"

namespace {
void DrawTextSelection(const Font& font, char* buffer, int bufferLength, int selectionStartI,
                       int selectionEndI, float x, float y, float maxWidth, float fontSize) {
  int selectionStart, selectionEnd;
  if (selectionStartI > selectionEndI) {
    selectionStart = selectionEndI;
    selectionEnd = selectionStartI;
  } else {
    selectionStart = selectionStartI;
    selectionEnd = selectionEndI;
  }


  float startX = x;
  float currentX = x;      // X position where the current line starts
  float currentY = y;      // Y position where the current line starts
  int lineStartIndex = 0;  // Start index of the current line in buffer
  bool endReached = false;

  bool insideSelection = false;
  for (int i = 0; i < bufferLength; i++) {
    if (i == selectionEnd) {
      endReached = true;
      const auto distFromStart = MeasureTextUpTo(buffer + lineStartIndex, i, font, fontSize, 0.5F);
      DrawRectangleRec({startX, y, x + distFromStart - startX, fontSize}, ColorAlpha(WHITE, 0.5F));
      break;
    }

    if (*buffer == '\n') {
      if (insideSelection) {
        const auto distFromStart = MeasureTextUpTo(buffer + lineStartIndex, i, font, fontSize, 0.5F);
        DrawRectangleRec({startX, y, x + distFromStart - startX, fontSize}, ColorAlpha(WHITE, 0.5F));
      }
      currentX = x;
      startX = x;
      lineStartIndex = i + 1;
      currentY += fontSize;
      continue;
    }

    if (i == selectionStart) {
      insideSelection = true;
      startX = x + MeasureTextUpTo(buffer + lineStartIndex, i, font, fontSize, 0.5F);
    }
  }
  if (!endReached) {
    const auto distFromStart = MeasureTextUpTo(buffer + lineStartIndex, bufferLength, font, fontSize, 0.5F);
    DrawRectangleRec({startX, y, x + distFromStart - startX, fontSize}, ColorAlpha(WHITE, 0.5F));
  }
}

}  // namespace

void TextInputField::draw(EditorContext& ec, Node& parent) {
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
  //Draw selection highlight
  if (isDragging && selectionStart != selectionEnd) {
    DrawTextSelection(font, buffer.data(), buffer.size(), selectionStart, selectionEnd, bounds.x + 3,
                      bounds.y, bounds.width, fs);
  }
}

void TextInputField::update(EditorContext& ec, Node& parent) {
  auto inText = inputs[0].getData<PinType::STRING>();
  if (inText != nullptr) buffer = inText;
  outputs[0].setData<PinType::STRING>(buffer.c_str());

  if (!isFocused) return;

  if (isDragging) {
    selectionEnd = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);
  }

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isDragging = true;
    cursorPos = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);
    selectionStart = cursorPos;
    selectionEnd = selectionStart;
  }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }

  if (ec.input.isKeyPressed(KEY_V) && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    buffer.insert(cursorPos, GetClipboardText());
    updateState(ec);
  }

  if (ec.input.isKeyPressed(KEY_C) && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    auto* data = buffer.data();
    int sStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
    int sEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
    const char temp = *(data + sEnd);
    SetClipboardText(data + sStart);
    *(data + sEnd) = temp;
  }

  int key = GetCharPressed();
  while (key > 0) {
    if (key >= 32 && key <= 125 && buffer.length() < 1024) {
      buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
      cursorPos++;
      updateState(ec);
    }
    key = GetCharPressed();  // Check next character in the queue
  }

  if (IsKeyPressed(KEY_ENTER)) {
    buffer.insert(buffer.begin() + cursorPos, '\n');
    cursorPos++;
    updateState(ec);
  }

  if (cursorPos > 0 && (ec.input.isKeyPressed(KEY_BACKSPACE) || ec.input.isKeyPressedRepeat(KEY_BACKSPACE))) {
    buffer.erase(buffer.begin() + cursorPos - 1);
    cursorPos--;
    updateState(ec);
  }

  if (cursorPos < buffer.size()
      && (ec.input.isKeyPressed(KEY_DELETE) || ec.input.isKeyPressedRepeat(KEY_DELETE))) {
    updateState(ec);
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

void TextInputField::onFocusGain(EditorContext& ec) {
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

void TextInputField::onFocusLoss(EditorContext& ec) {
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

void TextInputField::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label
  addPinInput(PinType::STRING);
  addPinOutput(PinType::STRING);
}

void TextInputField::save(FILE* file) {
  cxstructs::io_save(file, buffer.c_str());
}

void TextInputField::load(FILE* file) {
  cxstructs::io_load(file, buffer);
}

void TextInputField::updateState(EditorContext& ec) {
  int lineCount = 0;
  int lineStart = 0;
  float longestLine = 250;
  auto* text = buffer.data();
  const int size = static_cast<int>(buffer.size());

  for (int i = 0; i < size; ++i) {
    if (*(text + i) == '\n') {
      float lineWidth =
          MeasureTextUpTo(text + lineStart, i, ec.display.editorFont, ec.display.fontSize, 0.5F);
      longestLine = std::max(longestLine, lineWidth);
      lineStart = i + 1;
      lineCount++;
    }
  }

  if (lineStart < size) {
    float lineWidth =
        MeasureTextUpTo(text + lineStart, size - lineStart, ec.display.editorFont, ec.display.fontSize, 0.5F);
    longestLine = std::max(longestLine, lineWidth);
    lineCount++;
  }

  height = 20 * lineCount;
  width = longestLine;
}

uint16_t TextInputField::getIndexFromPos(const Font& font, const float fs, const Vector2 mouse) const {
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