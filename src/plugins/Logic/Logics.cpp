#include "Logics.h"

#include "components/AndGateC.h"
#include "components/BoolC.h"
#include "components/BoolDisplayC.h"
#include "components/ClockC.h"
#include "components/OrGateC.h"

void Logics::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<ClockC>("L_Clock");
  cr.registerComponent<BoolDisplayC>("L_Display");
  cr.registerComponent<OrGateC>("L_Or");
  cr.registerComponent<AndGateC>("L_And");

  // Flavours
  cr.registerComponent<BoolC<IN_AND_OUT>>("L_Bool");
  cr.registerComponent<BoolC<INPUT_ONLY>>("L_Bool_In");
  cr.registerComponent<BoolC<OUTPUT_ONLY>>("L_Bool_Out");
}

void Logics::registerNodes(NodeRegister& nr) {
  nr.registerNode("Logic Clock", {{"Clock", "L_Clock"}});
  nr.registerNode("Bool Display", {{"Display", "L_Display"}});
  nr.registerNode("AND Gate", {{"Gate", "L_And"}});
  nr.registerNode("OR Gate", {{"Gate", "L_Or"}});
  nr.registerNode("Bool",{{"Bool","L_Bool"}});
}