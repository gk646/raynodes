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

#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_

#include "component/Component.h"
#include "graphics/DropDown.h"

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

struct MathC final : Component {
  int selectedMode = ADD;
  DropDown dropDown;
  explicit MathC(const char* name) : Component(name, MATH_OPERATION, 250, 20) {}
  Component* clone() override { return new MathC(*this); };
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext& ec, Node& parent) override;
  void save(FILE* file) override;
  void load(FILE* file) override;

  void onCreate(EditorContext& ec, Node& parent) override;

 private:
  static double performOperation(double x, double y, MOperation op);
};

inline const char* MOperationToString(MOperation op) {
  switch (op) {
    case MOperation::ADD:
      return "Add";
    case MOperation::Subtract:
      return "Subtract";
    case MOperation::Multiply:
      return "Multiply";
    case MOperation::Divide:
      return "Divide";
    case MOperation::Modulo:
      return "Modulo";
    case MOperation::Power:
      return "Power";
    case MOperation::Sqrt:
      return "Sqrt";
    case MOperation::Log:
      return "Log";
    case MOperation::Ln:
      return "Ln";
    case MOperation::Sin:
      return "Sin";
    case MOperation::Cos:
      return "Cos";
    case MOperation::Tan:
      return "Tan";
    case MOperation::ASin:
      return "ASin";
    case MOperation::ACos:
      return "ACos";
    case MOperation::ATan:
      return "ATan";
    case MOperation::Sinh:
      return "Sinh";
    case MOperation::Cosh:
      return "Cosh";
    case MOperation::Tanh:
      return "Tanh";
    case MOperation::Exp:
      return "Exp";
    case MOperation::Abs:
      return "Abs";
    default:
      return "Unknown Operation";
  }
}

#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_MATHC_H_