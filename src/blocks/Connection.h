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

struct Connection {
  //Source
  Node& fromNode;
  Component& from;
  OutputPin& out;
  //Destination
  Node& toNode;
  Component& to;
  InputPin& in;
  Connection(Node& fromNode, Component& from, OutputPin& out, Node& toNode, Component& to, InputPin& in);
  [[nodiscard]] Vector2 getFromPos() const;
  [[nodiscard]] Vector2 getToPos() const;
  [[nodiscard]] Color getConnectionColor() const;
};

#endif  //RAYNODES_SRC_NODE_CONNECTION_H_