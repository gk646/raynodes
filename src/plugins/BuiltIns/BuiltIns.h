#ifndef BUILTINS_H
#define BUILTINS_H

#include "plugin/PluginInterface.h"

struct BuiltIns final : RaynodesPluginI {
  void registerComponents(ComponentRegister& cr) override;
  void registerNodes(NodeRegister& nr) override;
};

extern "C" EXPORT inline RaynodesPluginI* CreatePlugin() {
  return new BuiltIns();
}

#endif  //BUILTINS_H