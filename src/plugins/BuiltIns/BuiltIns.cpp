#include "BuiltIns.h"


#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/NumberInput.h"
#include "components/StringToNumberC.h"
#include "components/TextInputField.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent("MathOp", GetCreateFunc<MathC>());
  cr.registerComponent("Display", GetCreateFunc<DisplayC>());
  cr.registerComponent("StrToNum", GetCreateFunc<StringToNumberC>());
  cr.registerComponent("TextInput", GetCreateFunc<TextInputField>());
  cr.registerComponent("NumberInput", GetCreateFunc<NumberInput>());

}

void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {"MathOp"});
  nr.registerNode("TextField", {"TextInput"});
  nr.registerNode("StringToNum", {"StrToNum"});
  nr.registerNode("Display", {"Display"});
  nr.registerNode("NumberField", {"NumberInput"});
}