#ifndef BOOLC_H
#define BOOLC_H

#include "component/Component.h"
#include "ui/elements/Switch.h"

template <ComponentStyle style = IN_AND_OUT>
struct BoolC final : Component {
  Switch activeSwitch;
  explicit BoolC(const ComponentTemplate ct) : Component(ct, 75, 20) {}
  Component* clone() override { return new BoolC(*this); }
  void draw(EditorContext& ec, Node& parent) override {
    const auto bounds = getBounds();
    activeSwitch.bounds.x = bounds.x;
    activeSwitch.bounds.y = bounds.y;
    activeSwitch.draw(ec);
  }
  void update(EditorContext& ec, Node& parent) override {
    activeSwitch.update(ec, ec.logic.worldMouse);
    if constexpr (style == IN_AND_OUT || style == OUTPUT_ONLY) {
      outputs[0].setData<BOOLEAN>(activeSwitch.isActive());
    }
    if constexpr (style == IN_AND_OUT || style == INPUT_ONLY) {
      if (inputs[0].isConnected()) activeSwitch.isOn = inputs[0].getData<BOOLEAN>();
    }
  }
  void onCreate(EditorContext& ec, Node& parent) override {
    if constexpr (style == IN_AND_OUT || style == INPUT_ONLY) { addPinInput(BOOLEAN); }
    if constexpr (style == IN_AND_OUT || style == OUTPUT_ONLY) { addPinOutput(BOOLEAN); }
    const auto [x, y, width, height] = getBounds();
    activeSwitch.bounds = Rectangle{x, y, 40, height};
  }
};

#endif  //BOOLC_H