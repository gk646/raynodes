#include "Component.h"

#include <raylib.h>

Rectangle Component::getBounds() const {
  return {x, y, static_cast<float>(width), static_cast<float>(height)};
}