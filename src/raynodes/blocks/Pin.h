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

#ifndef RAYNODES_SRC_NODE_PIN_H_
#define RAYNODES_SRC_NODE_PIN_H_

#include "shared/fwd.h"
#include "blocks/Connection.h"

enum class PinType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
  VECTOR_3,
  VECTOR_2,
};

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
    case PinType::VECTOR_2:
      return "Vec2";
    case PinType::VECTOR_3:
      return "Vec3";
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
  explicit OutputPin(const PinType pt) : Pin{pt, OUTPUT, 0} {}
  [[nodiscard]] auto isConnectable(InputPin& other) const -> bool;
  template <PinType pt>
  void setData(auto val) {
    data.set<pt>(val);
  }
};

struct InputPin final : Pin {
  Connection* connection = nullptr;
  InputPin() = default;
  explicit InputPin(const PinType pt) : Pin{pt, INPUT, 0} {}
  [[nodiscard]] auto isConnectable(const OutputPin& other) const -> bool {
    return connection == nullptr && other.pinType == pinType;
  }
  template <PinType pt>
  [[nodiscard]] auto getData() const {
    if (connection) {
      return connection->out.data.get<pt>();
    }
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
  [[nodiscard]] bool isConnected() const { return connection != nullptr; }
};

#endif  //RAYNODES_SRC_NODE_PIN_H_