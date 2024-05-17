#ifndef CLOCKC_H
#define CLOCKC_H

#include "application/EditorContext.h"

struct ClockC final : Component {
  int delayMillis;
  explicit ClockC(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new ClockC(*this); }
  void draw(EditorContext& ec, Node& parent) override {}
  void update(EditorContext& ec, Node& parent) override {}
};

#endif  //CLOCKC_H