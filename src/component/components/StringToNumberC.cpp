#include "StringToNumberC.h"

void StringToNumberC::draw(EditorContext& ec, Node& parent) {}

void StringToNumberC::update(EditorContext& ec, Node& parent) {
  auto inData = inputs[0].getData<PinType::STRING>();

  outputs[0].setData<PinType::FLOAT>(inData ? std::atof(inData) : 0.0);
  outputs[1].setData<PinType::INTEGER>(inData ? std::atoi(inData) : 0L);
}

void StringToNumberC::onCreate(EditorContext& ec, Node& parent) {
  addInput(PinType::STRING);

  addOutput(PinType::FLOAT);
  addOutput(PinType::INTEGER);
}

void StringToNumberC::save(FILE* file) {
  /*Not needed*/
}

void StringToNumberC::load(FILE* file) {
  /*Not needed*/
}