#include "blocks/Pin.h"

#include <raylib.h>

#include "blocks/Connection.h"

Color Pin::getColor() const {
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