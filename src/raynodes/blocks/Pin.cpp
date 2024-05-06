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

#include "blocks/Pin.h"

#include "application/EditorContext.h"
#include "shared/rayutils.h"

Color Pin::getColor() const {
  switch (pinType) {
    case BOOLEAN:
      return BLUE;
    case STRING:
      return ORANGE;
    case INTEGER:
      return RED;
    case FLOAT:
      return SKYBLUE;
    case DATA:
      return PURPLE;
    case VECTOR_3:
      break;
    case VECTOR_2:
      break;
    case NODE:
      return VIOLET;
  }
  return RED;
}

void Pin::DrawPin(Pin& p, const Font& f, float x, float y, bool showText) {
  constexpr float pinRadius = PIN_SIZE / 2.0F;
  constexpr float textOff = PIN_SIZE * 1.5F;
  const auto middlePos = Vector2{x, y + pinRadius};

  if (p.direction == INPUT && static_cast<InputPin*>(&p)->connection == nullptr) {
    DrawCircleSector(middlePos,pinRadius,0,360,50,UI::COLORS[N_BACK_GROUND]);
    DrawCircleLinesV(middlePos, pinRadius, p.getColor());
  } else {
    DrawCircleV(middlePos, pinRadius, p.getColor());
  }
  if (showText) {
    const auto txt = TypeToString(p.pinType);
    const Vector2 textPos = {middlePos.x + (p.direction == INPUT ? -textOff : textOff),
                             middlePos.y - pinRadius};
    DrawCenteredText(f, txt, textPos, Pin::PIN_SIZE + 2, 0, UI::COLORS[UI_LIGHT]);
  }
  p.yPos = y + pinRadius;
}

bool Pin::UpdatePin(EditorContext& ec, Node& n, Component* c, Pin& p, float x) {
  constexpr float pinRadius = PIN_SIZE / 2.0F;
  if (CheckCollisionPointCircle(ec.logic.worldMouse, {x, p.yPos}, pinRadius)) {
    ec.logic.assignDraggedPin(x, p.yPos, n, c, p);
    ec.input.consumeKeyboard();
    ec.input.consumeMouse();
    return true;
  }
  return false;
}

auto OutputPin::isConnectable(InputPin& other) const -> bool {
  return other.pinType == pinType && other.connection == nullptr;
}