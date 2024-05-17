#ifndef SLIDER_H
#define SLIDER_H

#include <raylib.h>

#include "application/EditorContext.h"
class Slider {
  Rectangle bounds{};

  float currentValue = 0;
  float maxVal;
  void draw() const {
    const auto beamHeight = bounds.height / 3.0F;
    const auto beamY = bounds.y + (bounds.height - beamHeight) / 2.0F;
    DrawRectangleRec({bounds.x, beamY, bounds.width, beamHeight}, UI::COLORS[UI_LIGHT]);


  }
  void update() {

  }
};

#endif  //SLIDER_H