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
      return DARKGRAY;
    case VECTOR_4:
      return {25, 60, 62, 255};  // Dark Green
    case VECTOR_3:
      return {38, 92, 66, 255};  // Green
    case VECTOR_2:
      return {62, 137, 72, 255};  // Light Green
    case NODE:
      return VIOLET;
    case IMAGE:
      return {184, 111, 80, 255};  // Light Brown
  }
  return RED;
}

void Pin::DrawPin(Pin& p, const Font& f, const float x, const float y, const bool showText) {
  constexpr float pinRadius = PIN_SIZE / 2.0F;
  constexpr float textOff = PIN_SIZE * 1.5F;
  const auto middlePos = Vector2{x, y + pinRadius};

  if (p.direction == INPUT && static_cast<InputPin*>(&p)->connection == nullptr) {
    DrawCircleSector(middlePos, pinRadius, 0, 360, 36, UI::COLORS[N_BACK_GROUND]);
    DrawCircleLinesV(middlePos, pinRadius, p.getColor());
  } else {
    DrawCircleSector(middlePos, pinRadius, 0, 360, 36, p.getColor());
  }

  if (showText) [[unlikely]]{
    const auto txt = TypeToString(p.pinType);
    const Vector2 textPos = {middlePos.x + (p.direction == INPUT ? -textOff : textOff), middlePos.y - pinRadius};
    DrawCenteredText(f, txt, textPos, Pin::PIN_SIZE + 2, 0, UI::COLORS[UI_LIGHT]);
  }
  p.yPos = y + pinRadius;
}

bool Pin::UpdatePin(EditorContext& ec, Node& n, Component* c, Pin& p, const float x) {
  constexpr float pinRadius = PIN_SIZE / 2.0F;
  if (CheckCollisionPointCircle(ec.logic.worldMouse, {x, p.yPos}, pinRadius)) {
    ec.logic.assignDraggedPin(x, p.yPos, n, c, p);
    ec.input.consumeKeyboard();
    ec.input.consumeMouse();
    return true;
  }
  return false;
}

auto OutputPin::isConnectable(const EditorContext& ec, const InputPin& other) const -> bool {
  if (other.pinType != pinType) [[unlikely]] { return false; }

  if (pinType == NODE) [[unlikely]] {
    // Have to check if the connection already exists - we allow multiple node-to-node inputs
    for (const auto n : ec.core.connections) {
      if (&n->out == this && &n->in == &other) return false;
    }
    return true;
  }

  return other.connection == nullptr;
}