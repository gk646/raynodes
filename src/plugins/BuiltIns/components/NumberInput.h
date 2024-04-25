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

#ifndef NUMBERINPUT_H
#define NUMBERINPUT_H

#include <string>

#include "component/Component.h"

struct TextAction;
struct Font;
class NumberInput final : public Component {
  inline static uint8_t BLINK_DELAY = 45;
  std::string buffer;
  TextAction* currAction = nullptr;
  uint16_t cursorPos{};
  bool showCursor = false;
  uint8_t blinkCounter = 0;

 public:
  explicit NumberInput(const char* name) : Component(name, 150, 20) {}
  Component* clone() override { return new NumberInput(*this); }
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext&, Node& parent) override;
  void load(FILE* file) override;
  void save(FILE* file) override;
  void onFocusGain(EditorContext&) override;
  void onFocusLoss(EditorContext&) override;
  void onCreate(EditorContext& ec, Node& parent) override;
  const char* getString() override { return buffer.c_str(); }

 private:
  [[nodiscard]] uint16_t getIndexFromPos(const Font& font, float fs, Vector2 mouse) const;
};
#endif  //NUMBERINPUT_H