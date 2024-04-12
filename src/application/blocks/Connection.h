#ifndef RAYNODES_SRC_NODE_CONNECTION_H_
#define RAYNODES_SRC_NODE_CONNECTION_H_

#include "shared/fwd.h"

#include "Pin.h"

struct ConnectionData {
  union {
    char* string;
    int integer;
    bool boolean;
    float floating;
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
      static_assert(false, "Unsupported PinType");
    }
  }
};

struct Connection {
  Node& from;
  Node& to;
  Pin& in;
  Pin& out;
  ConnectionData data;
};

#endif  //RAYNODES_SRC_NODE_CONNECTION_H_
