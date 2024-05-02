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

#include "ui/TextField.h"

#include <cxutil/cxmath.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"
#include "shared/rayutils.h"

namespace {
void DrawTextSelection(const Font& f, char* buffer, int bufferLength, int selectionStartI, int selectionEndI,
                       float x, float y, float fs) {
  int selectionStart = std::min(selectionStartI, selectionEndI);
  int selectionEnd = std::max(selectionStartI, selectionEndI);

  float currentX = x;
  float currentY = y;
  int lineStartIndex = 0;

  for (int i = 0; i <= bufferLength; i++) {
    if (i == bufferLength || buffer[i] == '\n') {
      if (selectionStart <= i && selectionEnd > lineStartIndex) {
        float lineStartX =
            (selectionStart > lineStartIndex)
                ? MeasureTextUpTo(buffer + lineStartIndex, selectionStart - lineStartIndex, f, fs, 0.5F)
                : 0;
        float lineEndX =
            MeasureTextUpTo(buffer + lineStartIndex, std::min(selectionEnd, i) - lineStartIndex, f, fs, 0.5F);
        DrawRectangleRec({currentX + lineStartX, currentY, lineEndX - lineStartX, fs},
                         ColorAlpha(WHITE, 0.5F));
      }
      lineStartIndex = i + 1;
      currentX = x;
      currentY += fs;
    }
  }
}

void DrawCursor(const Font& f, char* buffer, int bufferLength, int cursorPos, float x, float y, float fs) {
  int lineCount = 0;
  int startIndex = 0;
  for (int i = 0; i < bufferLength; ++i) {
    if (i == cursorPos) break;
    if (buffer[i] == '\n') {
      lineCount++;
      startIndex = i + 1;
    }
  }

  float offset = MeasureTextUpTo(buffer + startIndex, cursorPos - startIndex, f, fs, 0.5F);
  DrawTextEx(f, "|", {x + offset, y + lineCount * fs}, fs, 0.5F, WHITE);
}

bool IsInputAllowed(const int key, InputConstraint constraint) {
  if (constraint == NUMERIC) {
    return (key >= 48 && key <= 57) || key == 46 /* dot*/;
  }
  if (constraint == NONE) {
    return key >= 32 && key <= 125;
  }

  return false;
}
}  // namespace

void TextInputField::draw() {
  //Safety measure
  cursorPos = cxstructs::clamp(static_cast<int>(cursorPos), 0, static_cast<int>(buffer.size()));

  //Main field
  DrawRectangleRec(bounds, isFocused ? DARKGRAY : GRAY);
  //Draw the text
  DrawTextEx(*font, buffer.c_str(), {bounds.x + 3, bounds.y}, fs, 0.5F, WHITE);
  //Draw cursor
  if (showCursor) {
    DrawCursor(*font, buffer.data(), static_cast<int>(buffer.size()), cursorPos, bounds.x, bounds.y, fs);
  }

  //Draw selection highlight
  if (selectionStart != selectionEnd) {
    DrawTextSelection(*font, buffer.data(), static_cast<int>(buffer.size()), selectionStart, selectionEnd,
                      bounds.x + 3, bounds.y, fs);
  }
}

void TextInputField::update(Vector2 mouse) {
  if (!isFocused) return;

  if (isDragging) {
    selectionEnd = getIndexFromPos(mouse);
    const auto [start, end] = getSelection();
    cursorPos = end;
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isDragging = true;
    cursorPos = getIndexFromPos(mouse);
    selectionStart = cursorPos;
    selectionEnd = selectionStart;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }

  if (IsKeyPressed(KEY_V) && IsKeyDown(KEY_LEFT_CONTROL) && buffer.length() < 1024) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos = start;
    }
    int prev = buffer.size();
    buffer.insert(cursorPos, GetClipboardText());
    updateDimensions(bounds);
    cursorPos = prev + (buffer.size() - prev);
  }

  if (IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_CONTROL)) {
    auto* data = buffer.data();
    const auto [start, end] = getSelection();
    const char temp = *(data + end);
    *(data + end) = '\0';
    SetClipboardText(data + start);
    *(data + end) = temp;
  }

  int key = GetCharPressed();
  while (key > 0) {
    selectionStart = selectionEnd;
    if (IsInputAllowed(key, constraint) && buffer.length() < 1024) {
      buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
      cursorPos++;
      updateDimensions(bounds);
    }
    key = GetCharPressed();  // Check next character in the queue
  }

  if (IsKeyDown(KEY_LEFT_CONTROL)) {
    if (IsKeyPressed(KEY_A)) {
      selectionStart = 0;
      selectionEnd = buffer.size();
    }
  }

  if (IsKeyPressed(KEY_ENTER) && buffer.length() < 1024) {
    buffer.insert(buffer.begin() + cursorPos, '\n');
    cursorPos++;
    updateDimensions(bounds);
  }

  if (cursorPos > 0 && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos -= end - start - 1;
    } else {
      buffer.erase(buffer.begin() + cursorPos - 1);
    }
    selectionStart = selectionEnd;
    cursorPos--;
    updateDimensions(bounds);
  }

  if (cursorPos < buffer.size() && (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE))) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos = start;
    } else {
      buffer.erase(buffer.begin() + cursorPos);
    }
    selectionStart = selectionEnd;
    updateDimensions(bounds);
  }

  if (cursorPos < buffer.length() && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))) {
    cursorPos++;
  }

  if (cursorPos > 0 && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))) {
    cursorPos--;
  }

  // Handle blinking cursor
  blinkCounter++;
  if (blinkCounter >= BLINK_DELAY) {
    showCursor = !showCursor;
    blinkCounter = 0;
  }
}

void TextInputField::onFocusGain(const Vector2 mouse) {
  //Position the cursor correct inside the text
  cursorPos = getIndexFromPos(mouse);
  selectionEnd = selectionStart;
  blinkCounter = 0;
  showCursor = true;
  isFocused = true;
}

void TextInputField::onFocusLoss() {
  showCursor = false;
  isFocused = false;
  selectionEnd = selectionStart;
}

void TextInputField::updateDimensions(Rectangle& bounds) {
  if (!growAutomatic) return;  // Dont grow if specified
  int lineCount = 0;
  int lineStart = 0;
  float longestLine = 250;
  auto* text = buffer.data();
  const int size = static_cast<int>(buffer.size());

  bool lastCharNewLine = size > 0 && text[size - 1] == '\n';

  for (int i = 0; i < size; ++i) {
    if (text[i] == '\n') {
      float lineWidth = MeasureTextUpTo(text + lineStart, i - lineStart, *font, fs, 0.5F);
      longestLine = std::max(longestLine, lineWidth);
      lineStart = i + 1;
      lineCount++;
    }
  }

  if (lineStart < size || lastCharNewLine) {
    float lineWidth = MeasureTextUpTo(text + lineStart, size - lineStart, *font, fs, 0.5F);
    longestLine = std::max(longestLine, lineWidth);
    lineCount++;
  }

  bounds.height = std::max(20, static_cast<uint16_t>(fs) * lineCount);
  bounds.width = longestLine;
}

uint16_t TextInputField::getIndexFromPos(const Vector2 mouse) {
  const float relX = mouse.x - bounds.x;
  const float relY = mouse.y - bounds.y;

  if (relX < 0) return 0;
  if (relX > bounds.width) return buffer.size();

  int currentLineStartIndex = 0;
  float currentLineTop = 0;
  char* text = buffer.data();

  for (int i = 0; i <= buffer.size(); ++i) {
    if (i == buffer.size() || *(text + i) == '\n') {
      if (relY >= currentLineTop && relY < currentLineTop + fs) {
        for (int j = currentLineStartIndex; j < i; ++j) {
          const auto currentWidth =
              MeasureTextUpTo(text + currentLineStartIndex, j - currentLineStartIndex + 1, *font, fs, 0.5F);
          if (currentWidth > relX) {
            return j;
          }
        }
        return i;
      }
      currentLineStartIndex = i + 1;
      currentLineTop += fs;
    }
  }

  return buffer.size();
}

Ints TextInputField::getSelection() const {
  return selectionStart < selectionEnd ? Ints{selectionStart, selectionEnd}
                                       : Ints{selectionEnd, selectionStart};
}