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

#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_

#include <cmath>
#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "component/Component.h"
#include "ui/elements/SimpleDropDown.h"

enum MOperation : uint8_t {
  ADD,       // Addition
  Subtract,  // Subtraction
  Multiply,  // Multiplication
  Divide,    // Division
  Modulo,    // Modulus operation
  Power,     // Exponentiation
  Sqrt,      // Square root
  Log,       // Logarithm (base 10)
  Ln,        // Natural logarithm (base e)
  Sin,       // Sine
  Cos,       // Cosine
  Tan,       // Tangent
  ASin,      // Arc sine
  ACos,      // Arc cosine
  ATan,      // Arc tangent
  Sinh,      // Hyperbolic sine
  Cosh,      // Hyperbolic cosine
  Tanh,      // Hyperbolic tangent
  Exp,       // Exponential function (e^x)
  Abs,       // Absolute value
  END,
};

inline const char* MOperationToString(const MOperation op) {
  switch (op) {
    case ADD:
      return "Add";
    case Subtract:
      return "Subtract";
    case Multiply:
      return "Multiply";
    case Divide:
      return "Divide";
    case Modulo:
      return "Modulo";
    case Power:
      return "Power";
    case Sqrt:
      return "Sqrt";
    case Log:
      return "Log";
    case Ln:
      return "Ln";
    case Sin:
      return "Sin";
    case Cos:
      return "Cos";
    case Tan:
      return "Tan";
    case ASin:
      return "ASin";
    case ACos:
      return "ACos";
    case ATan:
      return "ATan";
    case Sinh:
      return "Sinh";
    case Cosh:
      return "Cosh";
    case Tanh:
      return "Tanh";
    case Exp:
      return "Exp";
    case Abs:
      return "Abs";
    default:
      return "Unknown Operation";
  }
}

struct MathC final : Component {
  int selectedMode = 0;
  SimpleDropDown dropDown{};
  explicit MathC(const ComponentTemplate ct) : Component(ct, 100, 20) {}
  Component* clone() override { return new MathC(*this); }

  void draw(EditorContext& ec, Node& /**/) override {
    const auto bounds = getBounds();
    dropDown.draw(ec, bounds.x, bounds.y);
  }

  void update(EditorContext& ec, Node& /**/) override {
    selectedMode = dropDown.update(ec);
    double a = inputs[0].getData<FLOAT>();
    double b = inputs[1].getData<FLOAT>();

    const auto res = performOperation(a, b, static_cast<MOperation>(selectedMode));
    outputs[0].setData<FLOAT>(res);
  }

  void onCreate(EditorContext& /**/, Node& /**/) override {
    internalLabel = false;  //We don't want to draw our label

    addPinInput(FLOAT);
    addPinInput(FLOAT);

    addPinOutput(FLOAT);

    dropDown.items.reserve(END + 1);
    for (int i = 0; i < END; i++) {
      dropDown.items.emplace_back(MOperationToString(static_cast<MOperation>(i)));
    }

    dropDown.w = width;
    dropDown.h = height;
  }

  void save(FILE* file) override { cxstructs::io_save(file, selectedMode); }

  void load(FILE* file) override {
    cxstructs::io_load(file, selectedMode);
    dropDown.selectedIndex = selectedMode;
  }

  static double performOperation(double x, double y, MOperation op) {
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
        if (y != 0) return static_cast<int>(x) % static_cast<int>(y);
      case END:
        break;
    }
    return 0;
  }
};

#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_