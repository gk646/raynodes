#ifndef CLOCKC_H
#define CLOCKC_H

#include "application/EditorContext.h"
#include "ui/elements/Switch.h"
#include "ui/elements/TextField.h"

struct ClockC final : Component {
  TextField delayField;
  Switch activeSwitch;
  int delayMillis = 250;
  float delayBuilder = 0.0F;
  bool currentState = false;
  explicit ClockC(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new ClockC(*this); }
  void draw(EditorContext& ec, Node& parent) override {
    const auto bounds = getBounds();
    activeSwitch.bounds.x = bounds.x;
    activeSwitch.bounds.y = bounds.y;
    delayField.bounds.x = bounds.x + 50;
    delayField.bounds.y = bounds.y;

    activeSwitch.draw(ec);
    delayField.draw("Tick Delay...");

    const auto& f = ec.display.editorFont;
    const auto fs = ec.display.fontSize;
    DrawTextEx(f, "ms Cycle", {bounds.x + 132, bounds.y}, fs, 0.0F, UI::COLORS[UI_LIGHT]);
  }
  void update(EditorContext& ec, Node& parent) override {
    if (delayField.hasUpdate()) delayMillis = cxstructs::str_parse_int(delayField.buffer.c_str());

    if (inputs[0].isConnected()) {
      activeSwitch.isOn = inputs[0].getData<BOOLEAN>();
    } else {
      activeSwitch.update(ec, ec.logic.worldMouse);
    }

    if (activeSwitch.isActive()) { delayBuilder += 1000.0F / START_FPS; }

    if (static_cast<int>(delayBuilder) >= delayMillis) {
      currentState = !currentState;
      outputs[0].setData<BOOLEAN>(currentState);
      delayBuilder = 0.0F;
    }
    if (ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) delayField.onFocusGain(ec.logic.worldMouse);
    delayField.update(ec, ec.logic.worldMouse);
  }

  void onCreate(EditorContext& ec, Node& parent) override {
    const auto [x, y, width, height] = getBounds();
    activeSwitch.bounds = {x, y, 40, height};
    delayField.bounds = {x + 50, y, 80, height};
    delayField.font = &ec.display.editorFont;
    delayField.buffer = "250";
    delayField.growAutomatic = false;

    addPinInput(BOOLEAN);
    addPinOutput(BOOLEAN);
  }

  void onFocusGain(EditorContext& ec) override {}
  void onFocusLoss(EditorContext& ec) override { delayField.onFocusLoss(); }
};

#endif  //CLOCKC_H