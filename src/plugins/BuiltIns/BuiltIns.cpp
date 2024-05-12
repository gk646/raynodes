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
#include "components/NumberFieldC.h"
#include "components/SeparateXYC.h"
#include "components/SeparateXYZC.h"
#include "components/StringToNumberC.h"
#include "components/TextFieldC.h"
#include "components/Vec2C.h"
#include "components/Vec3C.h"

void BuiltIns::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<MathC>("BI_MathOp");
  cr.registerComponent<DisplayC>("BI_Display");
  cr.registerComponent<StringToNumberC>("BI_StrToNum");
  cr.registerComponent<SeparateXYZC>("BI_SeparateXYZ");
  cr.registerComponent<SeparateXYC>("BI_SeparateXY");

  // NumberField - different flavours
  cr.registerComponent<NumberFieldC<IN_AND_OUT>>("BI_Number");
  cr.registerComponent<NumberFieldC<INPUT_ONLY>>("BI_Number_In");
  cr.registerComponent<NumberFieldC<OUTPUT_ONLY>>("BI_Number_Out");

  // Textfield - different flavours
  cr.registerComponent<TextFieldC<IN_AND_OUT>>("BI_Text");
  cr.registerComponent<TextFieldC<INPUT_ONLY>>("BI_Text_In");
  cr.registerComponent<TextFieldC<OUTPUT_ONLY>>("BI_Text_Out");

  // Vector 3  - different flavours
  cr.registerComponent<Vec3C<IN_AND_OUT>>("BI_Vec3");
  cr.registerComponent<Vec3C<INPUT_ONLY>>("BI_Vec3_In");
  cr.registerComponent<Vec3C<OUTPUT_ONLY>>("BI_Vec3_Out");

  // Vector 2  - different flavours
  cr.registerComponent<Vec2C<IN_AND_OUT>>("BI_Vec2");
  cr.registerComponent<Vec2C<INPUT_ONLY>>("BI_Vec2_In");
  cr.registerComponent<Vec2C<OUTPUT_ONLY>>("BI_Vec2_Out");
}

// IMPORTANT :: DONT CHANGE ORDER -> Will invalidate all saves made
// TODO properly manage that -> maybe register with a number region?
void BuiltIns::registerNodes(NodeRegister& nr) {
  // Misc
  nr.registerNode("MathOp", {{"Operation", "BI_MathOp"}});
  nr.registerNode("Display", {{"Display", "BI_Display"}});

  // Conversion
  nr.registerNode("StringToNum", {{"Converter", "BI_StrToNum"}});
  nr.registerNode("SeparateXYZ", {{"Separator", "BI_SeparateXYZ"}});
  nr.registerNode("SeparateXY", {{"Separator", "BI_SeparateXY"}});

  // Data Input
  nr.registerNode("TextField", {{"Text", "BI_Text_Out"}});
  nr.registerNode("NumberField", {{"Number", "BI_Number_Out"}});
  nr.registerNode("Vector3", {{"Vec3", "BI_Vec3_Out"}});
  nr.registerNode("Vector2", {{"Vec2", "BI_Vec2_Out"}});

}