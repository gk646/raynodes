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

#include "QuestScript.h"

#include "components/DialogChoiceC.h"

void QuestScript::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<DialogChoiceC>("QST_DialogChoice");
}

void QuestScript::registerNodes(NodeRegister& nr) {
  nr.registerNode("Dialog Choice", {{"NPCType", "BI_Text_In"},
                                    {"DisplayText", "BI_Text_In"},
                                    {"Choice1", "QST_DialogChoice"},
                                    {"Choice2", "QST_DialogChoice"},
                                    {"Choice3", "QST_DialogChoice"},
                                    {"Choice4", "QST_DialogChoice"}});

  nr.registerNode("Dialog", {{"NPCType", "BI_Text_In"}, {"DisplayText", "BI_Text_In"}});

  nr.registerNode(
      "QuestHeader",
      {{"Name", "BI_Text_In"}, {"Description", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Level", "BI_Number_In"}});

  nr.registerNode("GOTO Position", {{"NPCType", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Target", "BI_Vec2_In"}});
  nr.registerNode(
      "GOTO Proximity",
      {{"NPCType", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Target", "BI_Vec2_In"}, {"Distance", "BI_Number_In"}});
}