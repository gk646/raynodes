#include "BuiltIns.h"

#include "application/EditorContext.h"

#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/StringToNumberC.h"
#include "components/TextInputField.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent("MathOp", GetCreateFunc<MathC>());
  cr.registerComponent("Display", GetCreateFunc<DisplayC>());
  cr.registerComponent("StrToNum", GetCreateFunc<StringToNumberC>());
  cr.registerComponent("TextInputField", GetCreateFunc<TextInputField>());
}

void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {"MathOp"});
  nr.registerNode("TextField", {"TextInputField"});
  nr.registerNode("StringToNum", {"StrToNum"});
  nr.registerNode("Display", {"Display"});
}