#include "Logics.h"

#include "components/ClockC.h"

void Logics::registerComponents(ComponentRegister& cr) {
  cr.registerComponent<ClockC>("L_Clock");
}

void Logics::registerNodes(NodeRegister& nr) {}