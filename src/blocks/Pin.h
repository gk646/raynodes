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

enum class Direction : bool { INPUT, OUTPUT };

struct Pin {
  static constexpr float PIN_SIZE = 20.0F;  //Should be to be cleanly divisible by 2
  Connection* connection;
  PinType pinType;
  Direction direction;
  uint8_t idx;  //The index of the pin inside its node

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
