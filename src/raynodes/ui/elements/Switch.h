#ifndef SWITCH_H
#define SWITCH_H

#include "shared/fwd.h"

#include <raylib.h>
struct EXPORT Switch {
  bool isOn = false;
  Rectangle bounds{};

  void draw(EditorContext& ec) const;
  void update(EditorContext& ec, Vector2 mouse);  // Pass worldMouse for worldSpace and mouse for screenspace
  [[nodiscard]] bool isActive() const { return isOn; }
};

#endif  //SWITCH_H