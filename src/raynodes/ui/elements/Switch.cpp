#include "Switch.h"

#include "application/EditorContext.h"

void Switch::draw(EditorContext& ec) const {
  DrawRectangleRec(bounds, DARKGRAY);

  DrawRectangleRec({bounds.x, bounds.y + bounds.height / 4.0F, bounds.width, bounds.height / 2.0F}, GRAY);

  if (isOn) {
    DrawRectangleRec({bounds.x + bounds.width / 2, bounds.y, bounds.width / 2, bounds.height}, GREEN);
    DrawRectangleRec(
        {bounds.x + bounds.width / 2, bounds.y + bounds.height / 4.0F, bounds.width / 2, bounds.height / 2.0F},
        GREEN);
  } else {
    DrawRectangleRec({bounds.x, bounds.y, bounds.width / 2, bounds.height}, RED);
    DrawRectangleRec({bounds.x, bounds.y + bounds.height / 4.0F, bounds.width / 2, bounds.height / 2.0F}, PINK);
  }

  DrawRectangleLinesEx(bounds, 1, BLACK);
}

void Switch::update(EditorContext& ec, Vector2 mouse) {
  if (ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) {
    if (CheckCollisionPointRec(mouse, bounds)) { isOn = !isOn; }
  }
}