#ifndef RAYNODES_SRC_NODE_CONNECTION_H_
#define RAYNODES_SRC_NODE_CONNECTION_H_

#include "shared/fwd.h"

#include "Pin.h"

struct ConnectionData {
  union {
    char* string;
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
};


struct Connection {
  //Source
  Node& from;
  Pin& out;
  //Destination
  Node& to;
  Pin& in;
  ConnectionData data;
  Connection(Node& from, Pin& out, Node& to, Pin& in)
      : from(from), out(out), to(to), in(in), data() {
    out.connection = this;
    in.connection = this;
  }
  Vector2 getFromPos() const;
  Vector2 getToPos()const;
  Color getConnectionColor() const;
};

#endif  //RAYNODES_SRC_NODE_CONNECTION_H_
