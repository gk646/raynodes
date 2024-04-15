#ifndef RAYNODES_SRC_NODE_PIN_H_
#define RAYNODES_SRC_NODE_PIN_H_

#include "shared/fwd.h"

#include "blocks/Connection.h"

inline const char* PinTypeToString(PinType pt) {
  switch (pt) {
    case PinType::BOOLEAN:
      return "Boolean";
    case PinType::STRING:
      return "String";
    case PinType::INTEGER:
      return "Integer";
    case PinType::DATA:
      return "Data";
    case PinType::FLOAT:
      return "Float";
    default:
      return "Unknown Type";
  }
}

enum Direction : bool { INPUT, OUTPUT };

struct Pin {
  static constexpr float PIN_SIZE = 10.0F;  //Should be to be cleanly divisible by 2
  Connection* connection = nullptr;
  float yPos;
  PinType pinType;
  Direction direction;

  Pin() = default;
  Pin(PinType pt, Direction dir) : pinType(pt), direction(dir) {}
  [[nodiscard]] auto isConnectable(Pin& other) const -> bool {
    if (direction == INPUT) {
      return other.direction == OUTPUT && other.pinType == pinType;
    } else {
      return other.direction == INPUT && other.pinType == pinType && other.connection == nullptr;
    }
  }
  [[nodiscard]] auto getColor() const -> Color;
  template <PinType pt>
  [[nodiscard]] auto getData() const {
    if (connection) {
      return connection->data.get<pt>();
    } else {
      if constexpr (pt == PinType::STRING) {
        return static_cast<const char*>(nullptr);
      } else if constexpr (pt == PinType::INTEGER) {
        return 0LL;
      } else if constexpr (pt == PinType::BOOLEAN) {
        return false;
      } else if constexpr (pt == PinType::FLOAT) {
        return 0.0;
      } else if constexpr (pt == PinType::DATA) {
        return nullptr;
      } else {
        static_assert(pt == PinType::STRING, "Unsupported PinType");
      }
    }
  }
  template <PinType pt>
  void setData(auto data) const {
    if (connection) {
      connection->data.set<pt>(data);
    }
  }
  [[nodiscard]] bool isConnected() const { return connection != nullptr; }
};
#endif  //RAYNODES_SRC_NODE_PIN_H_