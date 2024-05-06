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

#include "blocks/Pin.h"

#include <raylib.h>

Color Pin::getColor() const {
  switch (pinType) {
    case BOOLEAN:
      return BLUE;
    case STRING:
      return ORANGE;
    case INTEGER:
      return RED;
    case FLOAT:
      return SKYBLUE;
    case DATA:
      return PURPLE;
    case VECTOR_3:
      break;
    case VECTOR_2:
      break;
    case NODE:
      return VIOLET;
  }
  return RED;
}

auto OutputPin::isConnectable(InputPin& other) const -> bool {
  return other.pinType == pinType && other.connection == nullptr;
}