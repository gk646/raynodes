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

#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_

#include <raylib.h>
#include "component/Component.h"
#include "application/EditorContext.h"

struct DisplayC final : Component {
  explicit DisplayC(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new DisplayC(*this); };
  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();
    DrawRectangleRec(bounds, GRAY);

    const char* txt;
    if (inputs[0].isConnected()) {
      txt = inputs[0].getData<STRING>();
      DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
    } else if (inputs[1].isConnected()) {
      txt = ec.string.formatText("%f", inputs[1].getData<FLOAT>());
      DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
    } else if (inputs[2].isConnected()) {
      txt = ec.string.formatText("%lld", inputs[2].getData<INTEGER>());
      DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
    }
  }
  void update(EditorContext& ec, Node& /**/) override {
    outputs[0].setData<STRING>(inputs[0].getData<STRING>());
    outputs[1].setData<FLOAT>(inputs[1].getData<FLOAT>());
    outputs[2].setData<INTEGER>(inputs[2].getData<INTEGER>());
  }
  void onCreate(EditorContext& ec, Node& /**/) override {
    addPinInput(STRING);
    addPinInput(FLOAT);
    addPinInput(INTEGER);

    addPinOutput(STRING);
    addPinOutput(FLOAT);
    addPinOutput(INTEGER);
  }
};
#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_