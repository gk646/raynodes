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

#include <filesystem>

#include "application/EditorContext.h"
#include "plugin/PluginInterface.h"
#include "plugin/PluginLoader.h"

namespace {
// Registers a plugin in one go
void RegisterPlugin(EditorContext& ec, PluginContainer& pc) {
  char nameBuff[PLG_MAX_NAME_LEN];

  // Start
  cxstructs::str_pad(nameBuff, PLG_MAX_NAME_LEN, pc.name, '.', nullptr);
  printf("    > %s ", nameBuff);  // Print the initial loading message

  // Components - registered first
  int componentsRegistered = static_cast<int>(ec.templates.componentFactory.size());
  ComponentRegister cr(ec, pc);
  pc.plugin->registerComponents(cr);
  componentsRegistered = static_cast<int>(ec.templates.componentFactory.size()) - componentsRegistered;
  printf("Components: %s / ", ec.string.getPaddedNum(componentsRegistered));

  // Nodes
  int nodesRegistered = static_cast<int>(ec.templates.registeredNodes.size());
  NodeRegister nr(ec, pc);
  pc.plugin->registerNodes(nr);
  nodesRegistered = static_cast<int>(ec.templates.registeredNodes.size()) - nodesRegistered;
  printf("Nodes: %s / ", ec.string.getPaddedNum(nodesRegistered));

  // Errors
  printf("Errors: %s", ec.string.getPaddedNum(nr.getErrors() + cr.getErrors()));
  printf("\n");
  fflush(stdout);
  fflush(stderr);
}

}  // namespace

bool Plugin::loadPlugins(EditorContext& ec) {
  const char* basePath = ec.string.formatText("%s%s", ec.string.applicationDir, PLUGIN_PATH);
  const char* filter;
#if defined(_WIN32)
  filter = ".dll";
#else
  filter = ".so";
#endif
  printf("Loading raynodes plugins:\n");

  char nameBuff[PLG_MAX_NAME_LEN]{};
  for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
    if (entry.path().extension() == filter) {
      std::string fileName = entry.path().stem().string();  // Get the filename without extension
      TextCopy(nameBuff, fileName.c_str());
      auto dll = PluginLoader::GetPluginInstance(entry.path().string().c_str(), "CreatePlugin");
      if (!dll.plugin) {
        fprintf(stderr, "Failed to load plugin: %s\n", nameBuff);
        continue;
      }
      dll.name = cxstructs::str_dup(nameBuff);
      dll.plugin->onLoad(ec);
      plugins.push_back(dll);
    }
  }

  ec.plugin.sortPlugins();  // BuiltIns has to be first

  for (auto& dll : plugins) {
    RegisterPlugin(ec, dll);
  }

  cxstructs::str_pad(nameBuff, 4, ec.string.formatText("%d", static_cast<int>(plugins.size())), '.');
  printf("Loaded %s Plugin(s)\n", nameBuff);
  printf("=============================================================\n");

  return true;
}

void Plugin::sortPlugins() {
  std::ranges::sort(plugins, [](const PluginContainer& p1, const PluginContainer& p2) {
    return p1.plugin->priority < p2.plugin->priority;
  });
}