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

#include "TextOutC.h"


#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

void TextOutputC::draw(EditorContext& ec, Node& /**/) {
  auto bounds = getBounds();
  textField.bounds.x = bounds.x;
  textField.bounds.y = bounds.y;
  textField.draw();
}

void TextOutputC::update(EditorContext& ec, Node& parent) {
  outputs[0].setData<STRING>(textField.buffer.c_str());

  textField.update(ec.logic.worldMouse);

  height = static_cast<uint16_t>(textField.bounds.height);
  width = static_cast<uint16_t>(textField.bounds.width);
}

void TextOutputC::onFocusGain(EditorContext& ec) {
  textField.onFocusGain(ec.logic.worldMouse);

  delete currentAction;
  currentAction = new TextAction(textField.buffer, textField.buffer);
}

void TextOutputC::onFocusLoss(EditorContext& ec) {
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
    ec.core.addEditorAction(ec, currentAction);
    currentAction = nullptr;
  }
}

void TextOutputC::onCreate(EditorContext& ec, Node& /**/) {
  internalLabel = true;
  textField.font = &ec.display.editorFont;
  textField.fs = ec.display.fontSize;

  addPinOutput(STRING);
}

void TextOutputC::save(FILE* file) {
  cxstructs::io_save(file, textField.buffer.c_str());
}

void TextOutputC::load(FILE* file) {
  cxstructs::io_load(file, textField.buffer);
  textField.updateDimensions();
}