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
void RegisterPlugin(EditorContext& ec, RaynodesPluginI* plugin) {
  char nameBuff[Plugin::MAX_NAME_LEN];

  // Start
  cxstructs::str_pad(nameBuff, Plugin::MAX_NAME_LEN, plugin->name, '.', nullptr, ":");
  printf("    > %s ", nameBuff);  // Print the initial loading message

  // Nodes
  int nodesRegistered = static_cast<int>(ec.templates.nodeFactory.size());
  NodeRegister nr{ec, *plugin, 0};
  plugin->registerNodes(nr);
  nodesRegistered = static_cast<int>(ec.templates.nodeFactory.size()) - nodesRegistered;
  printf("Nodes: %s / ", String::GetPaddedNum(nodesRegistered));

  // Components
  int componentsRegistered = static_cast<int>(ec.templates.componentFactory.size());
  ComponentRegister cr{ec, *plugin, 0};
  plugin->registerComponents(cr);
  componentsRegistered = static_cast<int>(ec.templates.componentFactory.size()) - componentsRegistered;
  printf("Components: %s / ", String::GetPaddedNum(componentsRegistered));

  // Errors
  printf("Errors: %s", String::GetPaddedNum(nr.errorCount + cr.errorCount));
  printf("\n");
  fflush(stdout);
  fflush(stderr);
}

}  // namespace

bool Plugin::loadPlugins(EditorContext& ec) {
  const char* basePath = String::FormatText("%s%s", ec.string.applicationDir, PLUGIN_PATH);
  const char* filter;
#if defined(_WIN32)
  filter = ".dll";
#else
  filter = ".so";
#endif
  printf("Loading raynodes plugins:\n");

  char nameBuff[MAX_NAME_LEN]{};
  for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
    if (entry.path().extension() == filter) {
      std::string fileName = entry.path().stem().string();  // Get the filename without extension
      TextCopy(nameBuff, fileName.c_str());
      auto* plugin = PluginLoader::GetPluginInstance(entry.path().string().c_str(), "CreatePlugin");
      if (!plugin) {
        fprintf(stderr, "Failed to load plugin: %s\n", nameBuff);
        continue;
      }
      plugin->name = cxstructs::str_dup(nameBuff);
      plugin->onLoad(ec);
      plugins.push_back(plugin);
    }
  }

  ec.plugin.sortPlugins();  // BuiltIns has to be first

  for (auto* plugin : plugins) {
    RegisterPlugin(ec, plugin);
  }

  cxstructs::str_pad(nameBuff, 4, String::FormatText("%d", static_cast<int>(plugins.size())), '.');
  printf("Loaded %s Plugin(s)\n", nameBuff);
  printf("=============================================================\n");

  return true;
}

void Plugin::sortPlugins() {
  for (auto* plugin : plugins) {
    if (TextIsEqual(plugin->name, "BuiltIns")) {
      std::swap(plugins[0], plugin);
    }
  }
}