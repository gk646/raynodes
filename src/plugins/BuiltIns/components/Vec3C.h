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

#ifndef VEC3_H
#define VEC3_H

#include <array>
#include <string>

#include "application/EditorContext.h"
#include "ui/elements/TextField.h"

template <ComponentStyle style = IN_AND_OUT>
struct Vec3C final : Component {
  static constexpr int FLOAT_FIELDS = 3;
  TextField textFields[FLOAT_FIELDS]{};

  explicit Vec3C(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new Vec3C(*this); }

  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();

    constexpr std::array labels = {"X:", "Y:", "Z:"};
    const auto& font = ec.display.editorFont;
    const auto fs = ec.display.fontSize;
    float startX = bounds.x;

    for (int i = 0; i < FLOAT_FIELDS; ++i) {
      DrawTextEx(font, labels[i], {startX, bounds.y}, fs, 0.0F, UI::COLORS[UI_LIGHT]);
      startX += 12;
      auto& f = textFields[i];
      f.bounds.x = startX;
      f.bounds.y = bounds.y;
      f.draw();
      startX += f.bounds.width + 1;
    }

    width = startX - bounds.x;
  }

  void update(EditorContext& ec, Node& /**/) override {
    for (auto& f : textFields) {
      f.update(ec, ec.logic.worldMouse);
    }

    Vec3 out{0.0F, 0.0F, 0.0F};

    out.x = cxstructs::str_parse_float(textFields[0].buffer.c_str());
    out.y = cxstructs::str_parse_float(textFields[1].buffer.c_str());
    out.z = cxstructs::str_parse_float(textFields[2].buffer.c_str());

    outputs[0].setData<VECTOR_3>(out);

    if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (auto& f : textFields) {
        f.onFocusGain(ec.logic.worldMouse);
      }
    }
  }

  void onFocusGain(EditorContext& ec) override {
    for (auto& f : textFields) {
      f.onFocusGain(ec.logic.worldMouse);
    }
  }

  void onFocusLoss(EditorContext& ec) override {
    for (auto& f : textFields) {
      f.onFocusLoss();
    }
  }

  void onCreate(EditorContext& ec, Node& /**/) override {
    internalLabel = false;  //We don't want to draw our label
    for (auto& f : textFields) {
      f.font = &ec.display.editorFont;
      f.fs = ec.display.fontSize;
      f.minWidth = 60;
      f.bounds.width = 60;
      f.bounds.height = 20;
      f.buffer = "0";
    }

    // Correctly apply style
    if constexpr (style == IN_AND_OUT) {
      addPinOutput(VECTOR_3);
      addPinInput(VECTOR_3);
    } else if constexpr (style == INPUT_ONLY) {
      addPinInput(VECTOR_3);
    } else if constexpr (style == OUTPUT_ONLY) {
      addPinOutput(VECTOR_3);
    }
  }

  void save(FILE* file) override {
    float floats[FLOAT_FIELDS];
    for (int i = 0; i < FLOAT_FIELDS; ++i) {
      floats[i] = cxstructs::str_parse_float(textFields[i].buffer.c_str());
    }
    cxstructs::io_save(file, floats[0], floats[1], floats[2]);
  }

  void load(FILE* file) override {
    // This is done to make the low level saving and loading more straightforward and independent of our handling
    float floats[FLOAT_FIELDS];
    cxstructs::io_load(file, floats[0], floats[1], floats[2]);
    for (int i = 0; i < FLOAT_FIELDS; ++i) {
      cxstructs::str_embed_num(textFields[i].buffer, floats[i]);
    }
  }
};

#endif