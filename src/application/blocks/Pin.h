#ifndef RAYNODES_SRC_NODE_PIN_H_
#define RAYNODES_SRC_NODE_PIN_H_

#include "shared/fwd.h"

enum class PinType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
};

enum class Direction : bool { INPUT, OUTPUT };

struct Pin {
  PinType pinType;
  Direction direction;
  Connection* connection;
};
#endif  //RAYNODES_SRC_NODE_PIN_H_
