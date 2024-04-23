#include "BuiltIns.h"

#include "application/EditorContext.h"

#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/StringToNumberC.h"
#include "components/TextInputField.h"

void BuiltIns::registerComponents(EditorContext& ec) {
  ec.templates.registerComponent("MathOp", GetCreateFunc<MathC>());
  ec.templates.registerComponent("Display", GetCreateFunc<DisplayC>());
  ec.templates.registerComponent("StrConv", GetCreateFunc<StringToNumberC>());
  ec.templates.registerComponent("TextField", GetCreateFunc<TextInputField>());
}