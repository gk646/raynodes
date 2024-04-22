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

#include "application/EditorContext.h"
#include "component/components/DisplayC.h"

void DisplayC::draw(EditorContext& ec, Node& parent) {
  const auto bounds = getBounds();
  DrawRectangleRec(bounds, GRAY);

  const char* txt;
  if (inputs[0].isConnected()) {
    txt = inputs[0].getData<PinType::STRING>();
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  } else if (inputs[1].isConnected()) {
    txt = String::formatText("%f", inputs[1].getData<PinType::FLOAT>());
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  } else if (inputs[2].isConnected()) {
    txt = String::formatText("%d", inputs[2].getData<PinType::BOOLEAN>() == 1);
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  } else if (inputs[3].isConnected()) {
    txt = String::formatText("%d", inputs[3].getData<PinType::INTEGER>());
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  }
}

void DisplayC::update(EditorContext& ec, Node& parent) {
  outputs[0].setData<PinType::STRING>(inputs[0].getData<PinType::STRING>());
  outputs[1].setData<PinType::FLOAT>(inputs[1].getData<PinType::FLOAT>());
  outputs[2].setData<PinType::BOOLEAN>(inputs[2].getData<PinType::BOOLEAN>());
  outputs[3].setData<PinType::INTEGER>(inputs[3].getData<PinType::INTEGER>());
}

void DisplayC::onCreate(EditorContext& ec, Node& parent) {
  addPinInput(PinType::STRING);
  addPinInput(PinType::FLOAT);
  addPinInput(PinType::INTEGER);

  addPinOutput(PinType::STRING);
  addPinOutput(PinType::FLOAT);
  addPinOutput(PinType::INTEGER);
}

void DisplayC::save(FILE* file) {
  /*No state*/
}

void DisplayC::load(FILE* file) {
  /*No state*/
}