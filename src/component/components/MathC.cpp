#include "MathC.h"

#include <raylib.h>
#include <cmath>
#include <limits>

#include <cxutil/cxio.h>

#include "blocks/Connection.h"

void MathC::draw(EditorContext& ec, Node& parent) {
  auto bounds = getBounds();

  dropDown.draw(ec, bounds.x, bounds.y);
}

void MathC::update(EditorContext& ec, Node& parent) {
  selectedMode = dropDown.update(ec);
  double a = inputs[0].getData<PinType::FLOAT>();
  double b = inputs[1].getData<PinType::FLOAT>();

  auto res = performOperation(a, b, MOperation(selectedMode));
  outputs[0].setData<PinType::FLOAT>(res);
}

void MathC::onCreate(EditorContext& ec, Node& parent) {
  internalLabel = false;  //We don't want to draw our label

  addPinInput(PinType::FLOAT);
  addPinInput(PinType::FLOAT);

  addPinOutput(PinType::FLOAT);

  dropDown.items.reserve(MOperation::END + 1);
  for (int i = 0; i < MOperation::END; i++) {
    dropDown.items.emplace_back(MOperationToString(MOperation(i)));
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
    case MOperation::ADD:
      return x + y;
    case MOperation::Subtract:
      return x - y;
    case MOperation::Multiply:
      return x * y;
    case MOperation::Divide:
      return y != 0 ? x / y : std::numeric_limits<double>::infinity();  // Guard against division by zero
    case MOperation::Power:
      return std::pow(x, y);
    case MOperation::Sqrt:
      return std::sqrt(x);  // Note: sqrt typically takes one parameter
    case MOperation::Log:
      return std::log10(x);
    case MOperation::Ln:
      return std::log(x);
    case MOperation::Sin:
      return std::sin(x);
    case MOperation::Cos:
      return std::cos(x);
    case MOperation::Tan:
      return std::tan(x);
    case MOperation::ASin:
      return std::asin(x);
    case MOperation::ACos:
      return std::acos(x);
    case MOperation::ATan:
      return std::atan(x);
    case MOperation::Sinh:
      return std::sinh(x);
    case MOperation::Cosh:
      return std::cosh(x);
    case MOperation::Tanh:
      return std::tanh(x);
    case MOperation::Exp:
      return std::exp(x);
    case MOperation::Abs:
      return std::abs(x);
    case Modulo:
      return (int)x % (int)y;
  }
  return 0;
}