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

enum PinType : uint8_t {
  BOOLEAN,   // A boolean value
  STRING,    // A const char*
  INTEGER,   // A 64bit integer
  DATA,      // A arbitrary data pointer
  FLOAT,     // A single Float values
  VECTOR_3,  // 3 Float values
  VECTOR_2,  // 2 Float values
  NODE,      // Signals a connection between nodes
};

// Retain compile time knowledge of data types for now
// Runtime would introduce complexities but also problems:
//    - What if theres a connection and the type changes ? -> connection would have to be severed
//    - If this doesnt happen instantly it could lead to weird pointer access and errors
//        - Could be mitigated by solid handling

struct OutputData {
  union {
    const char* string;
    int64_t integer;
    bool boolean;
    double floating;
    Pointer pointer;
    Node* node;
    Vec2 vec2;
    Vec3 vec3;
    char padding[16];
  };

  template <PinType pt>
  auto get() {
    if constexpr (pt == STRING) {
      return string;
    } else if constexpr (pt == INTEGER) {
      return integer;
    } else if constexpr (pt == BOOLEAN) {
      return boolean;
    } else if constexpr (pt == FLOAT) {
      return floating;
    } else if constexpr (pt == DATA) {
      return pointer;
    } else if constexpr (pt == NODE) {
      return node;
    } else if constexpr (pt == VECTOR_2) {
      return vec2;
    } else if constexpr (pt == VECTOR_3) {
      return vec3;
    } else {
      static_assert(pt == STRING, "Unsupported PinType");
    }
  }
  template <PinType pt>
  void set(auto val) {
    if constexpr (pt == STRING) {
      string = val;
    } else if constexpr (pt == INTEGER) {
      integer = val;
    } else if constexpr (pt == BOOLEAN) {
      boolean = val;
    } else if constexpr (pt == FLOAT) {
      floating = val;
    } else if constexpr (pt == DATA) {
      pointer = val;
    } else if constexpr (pt == NODE) {
      node = val;
    } else if constexpr (pt == VECTOR_2) {
      vec2 = val;
    } else if constexpr (pt == VECTOR_3) {
      vec3 = val;
    } else {
      static_assert(pt == STRING, "Unsupported PinType");
    }
  }
};

enum Direction : bool { INPUT, OUTPUT };

struct Pin {
  static constexpr float PIN_SIZE = 10.0F;  //Should be cleanly divisible by 2
  const PinType pinType;
  const Direction direction;
  float yPos = 0;  //Y position - outputs are deterministically drawn on the right
  [[nodiscard]] Color getColor() const;
  static const char* TypeToString(PinType pt) {
    switch (pt) {
      case BOOLEAN:
        return "Boolean";
      case STRING:
        return "String";
      case INTEGER:
        return "Integer";
      case DATA:
        return "Data";
      case FLOAT:
        return "Float";
      case VECTOR_2:
        return "Vec2";
      case VECTOR_3:
        return "Vec3";
      case NODE:
        return "Node";
      default:
        return "Unknown Type";
    }
  }
  static void DrawPin(Pin& p, const Font& f, float x, float y, bool showText);
  static bool UpdatePin(EditorContext& ec, Node& n, Component* c, Pin& p, float x);
};

struct OutputPin final : Pin {
  OutputData data{nullptr};
  explicit OutputPin(const PinType pt) : Pin{pt, OUTPUT, 0} {}
  [[nodiscard]] auto isConnectable(InputPin& other) const -> bool;
  template <PinType pt>
  void setData(auto val) {
    data.set<pt>(val);
  }
};

struct InputPin final : Pin {
  Connection* connection = nullptr;
  explicit InputPin(const PinType pt) : Pin{pt, INPUT, 0} {}
  [[nodiscard]] auto isConnectable(const OutputPin& other) const -> bool {
    return connection == nullptr && other.pinType == pinType;
  }
  template <PinType pt>
  [[nodiscard]] auto getData() const {
    if (connection && pinType == pt) {  // Return dummy value on mismatch - safety measure
      return connection->out.data.get<pt>();
    }
    if constexpr (pt == STRING) {
      return static_cast<const char*>(nullptr);
    } else if constexpr (pt == INTEGER) {
      return static_cast<int64_t>(0);
    } else if constexpr (pt == BOOLEAN) {
      return false;
    } else if constexpr (pt == FLOAT) {
      return 0.0;
    } else if constexpr (pt == DATA) {
      return Pointer{nullptr, 0};
    } else if constexpr (pt == VECTOR_3) {
      return Vec3{0.0F, 0.0F, 0.0F};
    }else if constexpr (pt == VECTOR_2) {
      return Vec2{0.0F, 0.0F};
    } else {
      static_assert(pt == STRING, "Unsupported PinType");
    }
  }
  [[nodiscard]] bool isConnected() const { return connection != nullptr; }
};

#endif  //RAYNODES_SRC_NODE_PIN_H_