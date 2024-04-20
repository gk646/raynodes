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

struct Pointer {
  void* data;
  uint32_t size;
};

struct OutputData {
  union {
    const char* string;
    int64_t integer;
    uint64_t unsignedInt;
    bool boolean;
    double floating;
    Pointer pointer;  //Forces 16-bit size
    char padding[16];
  };

  template <PinType pt>
  auto get() {
    if constexpr (pt == PinType::STRING) {
      return string;
    } else if constexpr (pt == PinType::INTEGER) {
      return integer;
    } else if constexpr (pt == PinType::BOOLEAN) {
      return boolean;
    } else if constexpr (pt == PinType::FLOAT) {
      return floating;
    } else if constexpr (pt == PinType::DATA) {
      return pointer;
    } else {
      static_assert(pt == PinType::STRING, "Unsupported PinType");
    }
  }
  template <PinType pt>
  void set(auto val) {
    if constexpr (pt == PinType::STRING) {
      string = val;
    } else if constexpr (pt == PinType::INTEGER) {
      integer = val;
    } else if constexpr (pt == PinType::BOOLEAN) {
      boolean = val;
    } else if constexpr (pt == PinType::FLOAT) {
      floating = val;
    } else if constexpr (pt == PinType::DATA) {
      pointer = val;
    } else {
      static_assert(pt == PinType::STRING, "Unsupported PinType");
    }
  }
};

enum Direction : bool { INPUT, OUTPUT };

struct Pin {
  static constexpr float PIN_SIZE = 10.0F;  //Should be cleanly divisible by 2
  PinType pinType;
  Direction direction;
  float yPos = 0;  //Y position - outputs are deterministically drawn on the right
  [[nodiscard]] auto getColor() const -> Color;
};

struct OutputPin final : Pin {
  OutputData data{nullptr};
  OutputPin() = default;
  OutputPin(PinType pt) : Pin{pt, OUTPUT, 0} {}
  [[nodiscard]] auto isConnectable(InputPin& other) const -> bool;
  template <PinType pt>
  void setData(auto val) {
    data.set<pt>(val);
  }
};

struct InputPin final : Pin {
  Connection* connection = nullptr;
  InputPin() = default;
  InputPin(PinType pt) : Pin{pt, INPUT, 0} {}
  [[nodiscard]] auto isConnectable(OutputPin& other) const -> bool {
    return connection == nullptr && other.pinType == pinType;
  }
  template <PinType pt>
  [[nodiscard]] auto getData() const {
    if (connection) {
      return connection->out.data.get<pt>();
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
        return Pointer{nullptr, 0};
      } else {
        static_assert(pt == PinType::STRING, "Unsupported PinType");
      }
    }
  }
  [[nodiscard]] bool isConnected() const { return connection != nullptr; }
};

#endif  //RAYNODES_SRC_NODE_PIN_H_