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

#include "blocks/Connection.h"

Color Pin::getColor() const {
    switch (pinType) {
      case PinType::BOOLEAN:
        return BLUE;
      case PinType::STRING:
        return ORANGE;
      case PinType::INTEGER:
        return RED;
      case PinType::FLOAT:
        return SKYBLUE;
      case PinType::DATA:
        return PURPLE;
    }
    return RED;
}

auto OutputPin::isConnectable(InputPin& other) const -> bool {
    return other.pinType == pinType && other.connection == nullptr;
}