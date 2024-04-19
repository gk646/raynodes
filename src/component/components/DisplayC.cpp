#include "DisplayC.h"

#include <raylib.h>

#include "application/EditorContext.h"

void DisplayC::draw(EditorContext& ec, Node& parent) {
  const auto bounds = getBounds();
  DrawRectangleRec(bounds, LIGHTGRAY);

  const char* txt ;
  if (inputs[0].isConnected()) {
    txt = inputs[0].getData<PinType::STRING>();
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  }else if(inputs[1].isConnected()){
    txt = ec.string.formatText("%f", inputs[1].getData<PinType::FLOAT>());
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  }else if(inputs[2].isConnected()){
    txt = ec.string.formatText("%d", inputs[2].getData<PinType::BOOLEAN>() == 1);
    DrawTextEx(ec.display.editorFont, txt, {x, y}, ec.display.fontSize, 1.0F, WHITE);
  }else if(inputs[3].isConnected()){
    txt = ec.string.formatText("%d", inputs[3].getData<PinType::INTEGER>());
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
  addPinInput(PinType::BOOLEAN);
  addPinInput(PinType::INTEGER);

  addPinOutput(PinType::STRING);
  addPinOutput(PinType::FLOAT);
  addPinOutput(PinType::BOOLEAN);
  addPinOutput(PinType::INTEGER);
}

void DisplayC::save(FILE* file) {

}

void DisplayC::load(FILE* file) {

}