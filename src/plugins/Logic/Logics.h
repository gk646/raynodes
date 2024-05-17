#ifndef LOGIC_H
#define LOGIC_H

#include "plugin/PluginInterface.h"

// Accepting naming suggestions instead of logics

struct Logics final : RaynodesPluginI {
  void registerComponents(ComponentRegister& cr) override;
  void registerNodes(NodeRegister& nr) override;
};

extern "C" EXPORT RaynodesPluginI* CreatePlugin() {
  return new Logics();
}

#endif  //LOGIC_H