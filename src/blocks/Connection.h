#ifndef RAYNODES_SRC_NODE_CONNECTION_H_
#define RAYNODES_SRC_NODE_CONNECTION_H_

#include "shared/fwd.h"

enum class PinType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
};

struct ConnectionData {
  union {
    const char* string;
    int64_t integer;
    uint64_t unsignedInt;
    bool boolean;
    double floating;
    void* data;
    char pad[8];
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
      return data;
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
      data = val;
    } else {
      static_assert(pt == PinType::STRING, "Unsupported PinType");
    }
  }
};

struct Connection {
  //Source
  Component& from;
  Pin& out;
  //Destination
  Component& to;
  Pin& in;
  ConnectionData data;
  Connection(Component& from, Pin& out, Component& to, Pin& in);
  Vector2 getFromPos() const;
  Vector2 getToPos() const;
  Color getConnectionColor() const;
};

#endif  //RAYNODES_SRC_NODE_CONNECTION_H_