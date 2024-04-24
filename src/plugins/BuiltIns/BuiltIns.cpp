#include "BuiltIns.h"

#include "application/EditorContext.h"

#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/StringToNumberC.h"
#include "components/TextInputField.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent("MathOp", GetCreateFunc<MathC>());
  cr.registerComponent("Display", GetCreateFunc<DisplayC>());
  cr.registerComponent("StrConv", GetCreateFunc<StringToNumberC>());
  cr.registerComponent("TextField", GetCreateFunc<TextInputField>());
  cr.registerComponent("TextField", GetCreateFunc<TextInputField>());
}