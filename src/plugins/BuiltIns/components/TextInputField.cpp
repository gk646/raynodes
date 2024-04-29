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

#include <charconv>
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

template <InputConstraint constraint>
bool IsInputAllowed(const int key) {
  if constexpr (constraint == NUMERIC) {
    return (key >= 48 && key <= 57) || key == 46 /* dot*/;
  }
  if constexpr (constraint == NONE) {
    return key >= 32 && key <= 125;
  }

  return false;
}
}  // namespace

template <InputConstraint constraint>
void TextInputField<constraint>::draw(EditorContext& ec, Node& parent) {
  //Cache
  const auto bounds = getBounds();
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  //Safety measure
  cursorPos = cxstructs::clamp(static_cast<int>(cursorPos), 0, static_cast<int>(buffer.size()));

  //Main field
  DrawRectangleRec(bounds, isFocused ? DARKGRAY : GRAY);
  //Draw the text
  DrawTextEx(font, buffer.c_str(), {bounds.x + 3, bounds.y}, fs, 0.5F, WHITE);
  //Draw cursor
  if (showCursor) {
    DrawCursor(font, buffer.data(), static_cast<int>(buffer.size()), cursorPos, bounds.x, bounds.y, fs);
  }

  //Draw selection highlight
  if (selectionStart != selectionEnd) {
    DrawTextSelection(font, buffer.data(), static_cast<int>(buffer.size()), selectionStart, selectionEnd,
                      bounds.x + 3, bounds.y, fs);
  }
}

template <InputConstraint constraint>
void TextInputField<constraint>::update(EditorContext& ec, Node& parent) {
  const auto inText = inputs[0].getData<PinType::STRING>();
  if (inText != nullptr) buffer = inText;
  if constexpr (constraint == NONE) {
    outputs[0].setData<PinType::STRING>(buffer.c_str());
  } else if constexpr (constraint == NUMERIC) {
    outputs[0].setData<PinType::INTEGER>(cxstructs::str_parse_long(buffer.c_str()));
    outputs[1].setData<PinType::FLOAT>(cxstructs::str_parse_float(buffer.c_str()));
  }

  if (!isFocused) return;

  if (isDragging) {
    selectionEnd = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);
    const auto [start, end] = getSelection();
    cursorPos = end;
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

  if (ec.input.isKeyPressed(KEY_V) && ec.input.isKeyDown(KEY_LEFT_CONTROL) && buffer.length() < 1024) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos = start;
    }
    int prev = buffer.size();
    buffer.insert(cursorPos, GetClipboardText());
    updateState(ec);
    cursorPos = prev + (buffer.size() - prev);
  }

  if (ec.input.isKeyPressed(KEY_C) && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
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
    if (IsInputAllowed<constraint>(key) && buffer.length() < 1024) {
      buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
      cursorPos++;
      updateState(ec);
    }
    key = GetCharPressed();  // Check next character in the queue
  }

  if (ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    if (ec.input.isKeyPressed(KEY_A)) {
      selectionStart = 0;
      selectionEnd = buffer.size();
    }
  }

  if (ec.input.isKeyPressed(KEY_ENTER) && buffer.length() < 1024) {
    buffer.insert(buffer.begin() + cursorPos, '\n');
    cursorPos++;
    updateState(ec);
  }

  if (cursorPos > 0 && (ec.input.isKeyPressed(KEY_BACKSPACE) || ec.input.isKeyPressedRepeat(KEY_BACKSPACE))) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos -= end - start - 1;
    } else {
      buffer.erase(buffer.begin() + cursorPos - 1);
    }
    selectionStart = selectionEnd;
    cursorPos--;
    updateState(ec);
  }

  if (cursorPos < buffer.size()
      && (ec.input.isKeyPressed(KEY_DELETE) || ec.input.isKeyPressedRepeat(KEY_DELETE))) {
    if (selectionStart != selectionEnd) {
      const auto [start, end] = getSelection();
      buffer.erase(start, end - start);
      cursorPos = start;
    } else {
      buffer.erase(buffer.begin() + cursorPos);
    }
    selectionStart = selectionEnd;
    updateState(ec);
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

template <InputConstraint constraint>
void TextInputField<constraint>::onFocusGain(EditorContext& ec) {
  //Position the cursor correct inside the text
  cursorPos = getIndexFromPos(ec.display.editorFont, ec.display.fontSize, ec.logic.worldMouse);
  selectionEnd = selectionStart;
  blinkCounter = 0;
  showCursor = true;
  if (currAction) {
    delete currAction;
    currAction = nullptr;
  }
  currAction = new TextAction(buffer, buffer);
}

template <InputConstraint constraint>
void TextInputField<constraint>::onFocusLoss(EditorContext& ec) {
  showCursor = false;
  selectionEnd = selectionStart;
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

template <InputConstraint constraint>
void TextInputField<constraint>::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label
  if constexpr (constraint == NONE) {
    addPinInput(PinType::STRING);
    addPinOutput(PinType::STRING);
  } else {
    addPinInput(PinType::STRING);

    addPinOutput(PinType::INTEGER);
    addPinOutput(PinType::FLOAT);
  }
}

template <InputConstraint constraint>
void TextInputField<constraint>::save(FILE* file) {
  cxstructs::io_save(file, buffer.c_str());
}

template <InputConstraint constraint>
void TextInputField<constraint>::load(FILE* file) {
  cxstructs::io_load(file, buffer);
}

template <InputConstraint constraint>
void TextInputField<constraint>::updateState(EditorContext& ec) {
  int lineCount = 0;
  int lineStart = 0;
  float longestLine = 250;
  auto* text = buffer.data();
  const int size = static_cast<int>(buffer.size());

  bool lastCharNewLine = size > 0 && text[size - 1] == '\n';

  for (int i = 0; i < size; ++i) {
    if (text[i] == '\n') {
      float lineWidth =
          MeasureTextUpTo(text + lineStart, i - lineStart, ec.display.editorFont, ec.display.fontSize, 0.5F);
      longestLine = std::max(longestLine, lineWidth);
      lineStart = i + 1;
      lineCount++;
    }
  }

  if (lineStart < size || lastCharNewLine) {
    float lineWidth =
        MeasureTextUpTo(text + lineStart, size - lineStart, ec.display.editorFont, ec.display.fontSize, 0.5F);
    longestLine = std::max(longestLine, lineWidth);
    lineCount++;
  }

  height = std::max(20, static_cast<uint16_t>(ec.display.fontSize) * lineCount);
  width = longestLine;
}

template <InputConstraint constraint>
uint16_t TextInputField<constraint>::getIndexFromPos(const Font& font, const float fs, const Vector2 mouse) {
  const float relX = mouse.x - x;
  const float relY = mouse.y - y;

  if (relX < 0 ) return 0;
  if (relX > width ) return buffer.size();

  int currentLineStartIndex = 0;
  float currentLineTop = 0;
  char* text = buffer.data();

  for (int i = 0; i <= buffer.size(); ++i) {
    if (i == buffer.size() || *(text + i) == '\n') {
      if (relY >= currentLineTop && relY < currentLineTop + fs) {
        for (int j = currentLineStartIndex; j < i; ++j) {
          const auto currentWidth = MeasureTextUpTo(text + currentLineStartIndex, j - currentLineStartIndex + 1, font, fs, 0.5F);
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


template <InputConstraint constraint>
Ints TextInputField<constraint>::getSelection() const {
  return selectionStart < selectionEnd ? Ints{selectionStart, selectionEnd}
                                       : Ints{selectionEnd, selectionStart};
}

// Explicit template instantiation for NONE
template class TextInputField<NONE>;
template void TextInputField<NONE>::draw(EditorContext& ec, Node& parent);
template void TextInputField<NONE>::update(EditorContext& ec, Node& parent);
template void TextInputField<NONE>::onFocusGain(EditorContext& ec);
template void TextInputField<NONE>::onFocusLoss(EditorContext& ec);
template void TextInputField<NONE>::onCreate(EditorContext& ec, Node& parent);
template void TextInputField<NONE>::save(FILE* file);
template void TextInputField<NONE>::load(FILE* file);
template void TextInputField<NONE>::updateState(EditorContext& ec);
template Ints TextInputField<NONE>::getSelection() const;

// Explicit template instantiation for NUMERIC
template class TextInputField<NUMERIC>;
template void TextInputField<NUMERIC>::draw(EditorContext& ec, Node& parent);
template void TextInputField<NUMERIC>::update(EditorContext& ec, Node& parent);
template void TextInputField<NUMERIC>::onFocusGain(EditorContext& ec);
template void TextInputField<NUMERIC>::onFocusLoss(EditorContext& ec);
template void TextInputField<NUMERIC>::onCreate(EditorContext& ec, Node& parent);
template void TextInputField<NUMERIC>::save(FILE* file);
template void TextInputField<NUMERIC>::load(FILE* file);
template void TextInputField<NUMERIC>::updateState(EditorContext& ec);
template Ints TextInputField<NUMERIC>::getSelection() const;