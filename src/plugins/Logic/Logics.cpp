#include "Logics.h"

#include "components/AndGateC.h"
#include "components/BoolC.h"
#include "components/BoolDisplayC.h"
#include "components/ClockC.h"
#include "components/EqualGateC.h"
#include "components/NandGateC.h"
#include "components/NorGateC.h"
#include "components/NotGateC.h"
#include "components/OrGateC.h"
#include "components/XorGateC.h"

void Logics::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<ClockC>("L_Clock");
  cr.registerComponent<BoolDisplayC>("L_Display");
  cr.registerComponent<AndGateC>("L_And");
  cr.registerComponent<OrGateC>("L_Or");
  cr.registerComponent<XorGateC>("L_Xor");
  cr.registerComponent<EqualGateC>("L_Equal");
  cr.registerComponent<NandGateC>("L_Nand");
  cr.registerComponent<NorGateC>("L_Nor");
  cr.registerComponent<NotGateC>("L_Not");

  // Flavours
  cr.registerComponent<BoolC<IN_AND_OUT>>("L_Bool");
  cr.registerComponent<BoolC<INPUT_ONLY>>("L_Bool_In");
  cr.registerComponent<BoolC<OUTPUT_ONLY>>("L_Bool_Out");
}

void Logics::registerNodes(NodeRegister& nr) {
  // Dynamic
  nr.registerNode("Logic Clock", {{"Clock", "L_Clock"}});

  // Display
  nr.registerNode("Bool Display", {{"Display", "L_Display"}});

  // Getter setter
  nr.registerNode("Bool", {{"Bool", "L_Bool"}});

  // Gates
  nr.registerNode("AND Gate", {{"Gate", "L_And"}});
  nr.registerNode("OR Gate", {{"Gate", "L_Or"}});
  nr.registerNode("XOR Gate", {{"Gate", "L_Xor"}});
  nr.registerNode("Equal Gate", {{"Gate", "L_Equal"}});
  nr.registerNode("NAND Gate", {{"Gate", "L_Nand"}});
  nr.registerNode("NOR Gate", {{"Gate", "L_Nor"}});
  nr.registerNode("NOT Gate", {{"Gate", "L_Not"}});
}