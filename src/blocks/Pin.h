#ifndef RAYNODES_SRC_NODE_PIN_H_
#define RAYNODES_SRC_NODE_PIN_H_

#include "shared/fwd.h"

#include <raylib.h>

enum class PinType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
};

enum Direction : bool { INPUT, OUTPUT };

struct Pin {
  static constexpr float PIN_SIZE = 8.0F;  //Should be to be cleanly divisible by 2
  Connection* connection = nullptr;
  float yPos = 0;
  PinType pinType;
  Direction direction;

  Pin() = default;
  Pin(PinType pt, Direction dir) : pinType(pt), direction(dir) {}
  bool isConnectable(Pin& other) const {
    if (direction == INPUT) {
      return other.direction == OUTPUT && other.pinType == pinType;
    } else {
      return other.direction == INPUT && other.pinType == pinType && other.connection == nullptr;
    }
  }
  [[nodiscard]] Color getColor() const {
    switch (pinType) {
      case PinType::BOOLEAN:
        return BLUE;
      case PinType::STRING:
        return ORANGE;
      case PinType::INTEGER:
        return RED;
      case PinType::FLOAT:
        return SKYBLUE;
      case PinType::DATA:
        return PURPLE;
    }
    return RED;
  }
};
#endif  //RAYNODES_SRC_NODE_PIN_H_