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

#include "TextInputC.h"

#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

void TextInputC::draw(EditorContext& ec, Node& parent) {
  auto bounds = getBounds();
  textField.bounds.x = bounds.x;
  textField.bounds.y = bounds.y;
  textField.draw();
}

void TextInputC::update(EditorContext& ec, Node& parent) {
  const auto inText = inputs[0].getData<PinType::STRING>();
  if (inText != nullptr) textField.buffer = inText;
  if (textField.constraint == NONE) {
    outputs[0].setData<PinType::STRING>(textField.buffer.c_str());
  } else if (textField.constraint == NUMERIC) {
    const auto* text = textField.buffer.c_str();
    outputs[0].setData<PinType::INTEGER>(cxstructs::str_parse_long(text));
    outputs[1].setData<PinType::FLOAT>(cxstructs::str_parse_float(text));
  }

  textField.update(ec.logic.worldMouse);

  height = textField.bounds.height;
  width = textField.bounds.width;
}

void TextInputC::onFocusGain(EditorContext& ec) {
  textField.onFocusGain(ec.logic.worldMouse);

  delete currentAction;
  currentAction = new TextAction(textField.buffer, textField.buffer);
}

void TextInputC::onFocusLoss(EditorContext& ec) {
  textField.onFocusLoss();

  if (currentAction) {
    // Discard if nothing changed
    if (currentAction->beforeState == textField.buffer) {
      delete currentAction;
      currentAction = nullptr;
      return;
    }
    //TRANSFER OWNERSHIP
    currentAction->setAfter(textField.buffer);
    ec.core.addEditorAction(currentAction);
    currentAction = nullptr;
  }
}

void TextInputC::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label
  textField.font = &ec.display.editorFont;
  textField.fs = ec.display.fontSize;
  if (textField.constraint == NONE) {
    addPinInput(PinType::STRING);
    addPinOutput(PinType::STRING);
  } else {
    addPinInput(PinType::STRING);

    addPinOutput(PinType::INTEGER);
    addPinOutput(PinType::FLOAT);
  }
}

void TextInputC::save(FILE* file) {
  cxstructs::io_save(file, textField.buffer.c_str());
}

void TextInputC::load(FILE* file) {
  cxstructs::io_load(file, textField.buffer);
}