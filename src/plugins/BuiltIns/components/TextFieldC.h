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

#ifndef TEXTINPUTC_H
#define TEXTINPUTC_H

#include <string>

#include "application/EditorContext.h"
#include "component/Component.h"
#include "application/elements/Action.h"
#include "ui/elements/TextField.h"

template <ComponentStyle style = IN_AND_OUT>
class TextFieldC final : public Component {
  TextField textField;
  TextAction* currentAction = nullptr;

 public:
  explicit TextFieldC(const ComponentTemplate ct) : Component(ct, 200, 20), textField(200, 20, NONE) {}
  Component* clone() override { return new TextFieldC(*this); }
  void draw(EditorContext& ec, Node& /**/) override {
    auto bounds = getBounds();
    textField.bounds.x = bounds.x;
    textField.bounds.y = bounds.y;
    textField.draw();
  }

  void update(EditorContext& ec, Node& parent) override {
    textField.update(ec, ec.logic.worldMouse);

    if constexpr (style == IN_AND_OUT || style == INPUT_ONLY) {
      auto* input = inputs[0].getData<STRING>();
      if (input) textField.buffer = input;
    }

    if constexpr (style == IN_AND_OUT || style == OUTPUT_ONLY) {
      outputs[0].setData<STRING>(textField.buffer.c_str());
    }

    height = static_cast<uint16_t>(textField.bounds.height);
    width = static_cast<uint16_t>(textField.bounds.width);
  }

  void onFocusGain(EditorContext& ec) override {
    textField.onFocusGain(ec.logic.worldMouse);

    delete currentAction;
    currentAction = new TextAction(textField.buffer, textField.buffer);
  }

  void onFocusLoss(EditorContext& ec) override {
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

  void onCreate(EditorContext& ec, Node& /**/) override {
    internalLabel = false;  //We don't want to draw our label
    textField.font = &ec.display.editorFont;
    textField.fs = ec.display.fontSize;

    if constexpr (style == IN_AND_OUT || style == INPUT_ONLY) { addPinInput(STRING); }
    if constexpr (style == IN_AND_OUT || style == OUTPUT_ONLY) { addPinOutput(STRING); }
  }

  void save(FILE* file) override { cxstructs::io_save(file, textField.buffer.c_str()); }

  void load(FILE* file) override {
    cxstructs::io_load(file, textField.buffer);
    textField.updateDimensions();
  }

  const char* getString() override { return textField.buffer.c_str(); }
};

#endif  //TEXTINPUTC_H