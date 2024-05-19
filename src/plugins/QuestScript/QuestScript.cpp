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
  nr.registerNode("Dialog Proximity",
                  {{"NPCType", "BI_Text_In"}, {"DisplayText", "BI_Text_In"}, {"Distance", "BI_Number_In"}});

  nr.registerNode(
      "QuestHeader",
      {{"Name", "BI_Text_In"}, {"Description", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Level", "BI_Number_In"}});

  nr.registerNode("GOTO Position", {{"NPCType", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Target", "BI_Vec2_In"}});
  nr.registerNode(
      "GOTO Proximity",
      {{"NPCType", "BI_Text_In"}, {"Zone", "BI_Text_In"}, {"Target", "BI_Vec2_In"}, {"Distance", "BI_Number_In"}});

  nr.registerNode("MOVE NPC", {{"NPCType", "BI_Text_In"},
                               {"Waypoint1", "BI_Vec2_In"},
                               {"Waypoint2", "BI_Vec2_In"},
                               {"Waypoint3", "BI_Vec2_In"},
                               {"Waypoint4", "BI_Vec2_In"},
                               {"Waypoint5", "BI_Vec2_In"}});

  nr.registerNode("KILL Monster", {{"MonsterType", "BI_Text_In"}, {"Amount", "BI_Number_In"}});
  nr.registerNode("KILL Monsters", {{"MonsterType", "BI_Text_In"},
                                    {"Amount", "BI_Number_In"},
                                    {"MonsterType1", "BI_Text_In"},
                                    {"Amount1", "BI_Number_In"},
                                    {"MonsterType2", "BI_Text_In"},
                                    {"Amount2", "BI_Number_In"}});

  nr.registerNode(
      "Tile Change",
      {{"Zone", "BI_Text_In"}, {"Position", "BI_Vec2_In"}, {"Layer", "BI_Number_In"}, {"New Tile", "BI_Number_In"}});

  nr.registerNode("SPAWN", {{"Type", "BI_Text_In"},
                            {"Zone", "BI_Text_In"},
                            {"Level", "BI_Number_In"},
                            {"Position", "BI_Vec2_In"},
                            {"Position2", "BI_Vec2_In"},
                            {"Position3", "BI_Vec2_In"}});

  nr.registerNode("Set Quest Var", {{"Type", "BI_Number_In"}, {"Value", "BI_Number_In"}});
  nr.registerNode("SKIP", {{}});
  nr.registerNode("COLLECT Item", {{"ItemID", "BI_Number_In"}, {"ItemType", "BI_Number_In"}});
  nr.registerNode("WAIT", {{"Ticks", "BI_Number_In"}});
  nr.registerNode("DESPAWN Npc", {{"NPCType", "BI_Text_In"}});
  nr.registerNode("Combat Trigger", {{"Normal Chain", "BI_Node_Out"}, {"CombatStart Chain", "BI_Node_Out"}});
  nr.registerNode("Custom Scripted", {{"ID", "BI_Number_In"}});
}