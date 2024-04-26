#include "BuiltIns.h"

#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/NumberInput.h"
#include "components/StringToNumberC.h"
#include "components/TextInputField.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<MathC>("MathOp");
  cr.registerComponent<DisplayC>("Display");
  cr.registerComponent<StringToNumberC>("StrToNum");
  cr.registerComponent<TextInputField>("TextInput");
  cr.registerComponent<NumberInput>("NumberInput");
}

void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {{"Operation", "MathOp"}});
  nr.registerNode("TextField", {{"TextField", "TextInput"}});
  nr.registerNode("StringToNum", {{"Converter", "StrToNum"}});
  nr.registerNode("Display", {{"Display", "Display"}});
  nr.registerNode("NumberField", {{"Number", "NumberInput"}});
}