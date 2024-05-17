#ifndef ANDGATEC_H
#define ANDGATEC_H

#include "component/Component.h"
#include "application/EditorContext.h"

struct AndGateC final : Component {
  explicit AndGateC(const ComponentTemplate ct) : Component(ct, 75, 20) {}
  Component* clone() override { return new AndGateC(*this); }

  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();
    DrawRectangleRec(bounds, UI::COLORS[UI_MEDIUM]);
  }

  void update(EditorContext& ec, Node& /**/) override {
    const bool val = inputs[0].getData<BOOLEAN>() && inputs[1].getData<BOOLEAN>();
    outputs[0].setData<BOOLEAN>(val);
  }

  void onCreate(EditorContext& ec, Node& /**/) override {
    addPinInput(BOOLEAN);
    addPinInput(BOOLEAN);
    addPinOutput(BOOLEAN);
  }
};
#endif  //ANDGATEC_H