#ifndef BOOLDISPLAY_H
#define BOOLDISPLAY_H

#include "component/Component.h"
#include "application/EditorContext.h"

struct BoolDisplayC final : Component {
  explicit BoolDisplayC(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new BoolDisplayC(*this); };
  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();
    DrawRectangleRec(bounds, UI::Lighten(UI::COLORS[UI_MEDIUM], 25));

    if (inputs[0].isConnected()) {
      const bool active = inputs[0].getData<BOOLEAN>();
      DrawTextEx(ec.display.editorFont, active ? "TRUE" : "FALSE", {x, y}, ec.display.fontSize, 1.0F, WHITE);
    }
  }
  void update(EditorContext& ec, Node& /**/) override {
    outputs[0].setData<BOOLEAN>(inputs[0].getData<BOOLEAN>());
  }
  void onCreate(EditorContext& ec, Node& /**/) override {
    addPinInput(BOOLEAN);
    addPinOutput(BOOLEAN);
  }
};
#endif  //BOOLDISPLAY_H