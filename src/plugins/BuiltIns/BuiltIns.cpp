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
#include "components/NumberOutputC.h"
#include "components/SeparateXYZC.h"
#include "components/StringToNumberC.h"
#include "components/TextInputC.h"
#include "components/TextOutC.h"
#include "components/Vec3OutC.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<MathC>("BI_MathOp");
  cr.registerComponent<DisplayC>("BI_Display");
  cr.registerComponent<StringToNumberC>("BI_StrToNum");
  cr.registerComponent<TextOutputC>("BI_TextOut");
  cr.registerComponent<TextInputC>("BI_TextIn");
  cr.registerComponent<NumberOutputC>("BI_NumberOut");
  cr.registerComponent<SeparateXYZC>("BI_SeparateXYZ");
  cr.registerComponent<Vec3OutC>("BI_Vec3Out");
}

void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {{"Operation", "BI_MathOp"}});
  nr.registerNode("TextField", {{"TextField", "BI_TextOut"}});
  nr.registerNode("StringToNum", {{"Converter", "BI_StrToNum"}});
  nr.registerNode("Display", {{"Display", "BI_Display"}});
  nr.registerNode("NumberField", {{"Number", "BI_NumberOut"}});
  nr.registerNode("SeparateXYZ", {{"Vector", "BI_SeparateXYZ"}});
  nr.registerNode("Vector 3", {{"Vector3", "BI_Vec3Out"}});
}