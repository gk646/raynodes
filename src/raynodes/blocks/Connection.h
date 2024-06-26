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

#ifndef RAYNODES_SRC_NODE_CONNECTION_H_
#define RAYNODES_SRC_NODE_CONNECTION_H_

#include "shared/fwd.h"

struct EXPORT Connection final {
  //Source
  Node& fromNode;
  Component* from;  // NULL when connection from node to node
  OutputPin& out;
  //Destination
  Node& toNode;  // NULL when connection from node to node
  Component* to;
  InputPin& in;
  Connection(Node& fromNode, Component* from, OutputPin& out, Node& toNode, Component* to, InputPin& in);
  [[nodiscard]] Vector2 getFromPos() const;
  [[nodiscard]] Vector2 getToPos() const;
  [[nodiscard]] Color getConnectionColor() const;
  [[nodiscard]] bool isVisible() const;
  void close() const;
  void open();
};

#endif  //RAYNODES_SRC_NODE_CONNECTION_H_