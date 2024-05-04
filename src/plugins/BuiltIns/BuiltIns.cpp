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

#include "BuiltIns.h"

#include "components/MathC.h"
#include "components/DisplayC.h"
#include "components/StringToNumberC.h"
#include "components/TextInputC.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<MathC>("MathOp");
  cr.registerComponent<DisplayC>("Display");
  cr.registerComponent<StringToNumberC>("StrToNum");
  cr.registerComponent<TextInputC>("TextInput");
  cr.registerComponent<TextInputC>("NumberInput");
}

void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {{"Operation", "MathOp"}});
  nr.registerNode("TextField", {{"TextField", "TextInput"}});
  nr.registerNode("StringToNum", {{"Converter", "StrToNum"}});
  nr.registerNode("Display", {{"Display", "Display"}});
  nr.registerNode("NumberField", {{"Number", "NumberInput"}});
}