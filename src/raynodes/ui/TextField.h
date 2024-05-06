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

#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "shared/fwd.h"

#include <raylib.h>
#include <string>

enum InputConstraint : uint8_t {
  NONE,
  NUMERIC,
};

struct TextInputField final {
  inline static uint8_t BLINK_DELAY = 45;

  std::string buffer;
  Rectangle bounds;
  const Font* font = nullptr;
  float fs = 15.0F;  // Font size
  uint16_t cursorPos = 0;
  uint16_t selectionStart = 0;
  uint16_t selectionEnd = 0;
  uint16_t minWidth = 0;
  bool isDragging = false;
  bool showCursor = false;
  bool isFocused = false;
  bool growAutomatic = true;
  uint8_t blinkCounter = 0;
  const InputConstraint constraint = NONE;

  TextInputField() = default;
  explicit TextInputField(float w, float h, InputConstraint constraint = NONE)
      : bounds(0, 0, w, h), minWidth(static_cast<uint16_t>(w)), constraint(constraint) {}
  void draw();
  void update(Vector2 mouse);
  void onFocusGain(Vector2 mouse);
  void onFocusLoss();
  void updateDimensions();

 private:
  [[nodiscard]] Ints getSelection() const;
  [[nodiscard]] uint16_t getIndexFromPos(Vector2 mouse);
};

#endif  //TEXTFIELD_H