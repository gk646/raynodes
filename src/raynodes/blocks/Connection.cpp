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

#include "blocks/Connection.h"
#include "blocks/Pin.h"
#include <raylib.h>

#include "node/Node.h"

Connection::Connection(Node& fromNode, Component& from, OutputPin& out, Node& toNode, Component& to,
                       InputPin& in)
    : fromNode(fromNode), from(from), out(out), toNode(toNode), to(to), in(in) {}

Vector2 Connection::getFromPos() const {
  return {fromNode.x + fromNode.width, out.yPos};
}
Vector2 Connection::getToPos() const {
  return {to.x, in.yPos};
}
void Connection::close() const {
  in.connection = nullptr;
}
void Connection::open() {
  in.connection = this;
}