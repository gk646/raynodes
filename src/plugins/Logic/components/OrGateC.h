#ifndef ORGATEC_H
#define ORGATEC_H


#include "component/Component.h"
#include "application/EditorContext.h"

struct OrGateC final : Component {
  explicit OrGateC(const ComponentTemplate ct) : Component(ct, 75, 20) {}
  Component* clone() override { return new OrGateC(*this); }

  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();
    DrawRectangleRec(bounds, UI::COLORS[UI_MEDIUM]);
  }

  void update(EditorContext& ec, Node& /**/) override {
    const bool val = inputs[0].getData<BOOLEAN>() || inputs[1].getData<BOOLEAN>();
    outputs[0].setData<BOOLEAN>(val);
  }

  void onCreate(EditorContext& ec, Node& /**/) override {
    addPinInput(BOOLEAN);
    addPinInput(BOOLEAN);
    addPinOutput(BOOLEAN);
  }
};

#endif //ORGATEC_H