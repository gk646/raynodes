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
//

#include "MathC.h"

#include <raylib.h>
#include <cmath>
#include <limits>

#include <cxutil/cxio.h>

#include "blocks/Connection.h"

void MathC::draw(EditorContext& ec, Node& parent) {
  const auto bounds = getBounds();
  dropDown.draw(ec, bounds.x, bounds.y);
}

void MathC::update(EditorContext& ec, Node& parent) {
  selectedMode = dropDown.update(ec);
  double a = inputs[0].getData<PinType::FLOAT>();
  double b = inputs[1].getData<PinType::FLOAT>();

  const auto res = performOperation(a, b, static_cast<MOperation>(selectedMode));
  outputs[0].setData<PinType::FLOAT>(res);
}

void MathC::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label

  addPinInput(PinType::FLOAT);
  addPinInput(PinType::FLOAT);

  addPinOutput(PinType::FLOAT);

  dropDown.items.reserve(END + 1);
  for (int i = 0; i < END; i++) {
    dropDown.items.emplace_back(MOperationToString(static_cast<MOperation>(i)));
  }

  dropDown.w = width;
  dropDown.h = height;
}

void MathC::save(FILE* file) {
  cxstructs::io_save(file, selectedMode);
}

void MathC::load(FILE* file) {
  cxstructs::io_load(file, selectedMode);
}

double MathC::performOperation(double x, double y, MOperation op) {
  switch (op) {
    case ADD:
      return x + y;
    case Subtract:
      return x - y;
    case Multiply:
      return x * y;
    case Divide:
      return y != 0 ? x / y : std::numeric_limits<double>::infinity();  // Guard against division by zero
    case Power:
      return std::pow(x, y);
    case Sqrt:
      return std::sqrt(x);  // Note: sqrt typically takes one parameter
    case Log:
      return std::log10(x);
    case Ln:
      return std::log(x);
    case Sin:
      return std::sin(x);
    case Cos:
      return std::cos(x);
    case Tan:
      return std::tan(x);
    case ASin:
      return std::asin(x);
    case ACos:
      return std::acos(x);
    case ATan:
      return std::atan(x);
    case Sinh:
      return std::sinh(x);
    case Cosh:
      return std::cosh(x);
    case Tanh:
      return std::tanh(x);
    case Exp:
      return std::exp(x);
    case Abs:
      return std::abs(x);
    case Modulo:
      if (y != 0) return (int)x % (int)y;
    case END:
      break;
  }
  return 0;
}