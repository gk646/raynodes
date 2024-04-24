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

#include <string>
#include <filesystem>
#include <raylib.h>  //Cause of rayutils

#include "shared/rayutils.h"
#include "application/EditorContext.h"
#include "plugin/PluginLoader.h"

String::String() {
  applicationDir = GetApplicationDirectory();
}

void Plugin::startLoad(EditorContext& ec, const RaynodesPluginI* plugin) {
  char nameBuff[MAX_NAME_LEN];
  PadWithChar(nameBuff, MAX_NAME_LEN, plugin->name, '.', nullptr, ":");
  printf("    > %s ", nameBuff);  // Print the initial loading message
  previousCompSize = static_cast<int>(ec.templates.componentFactory.size());
  previousNodeSize = static_cast<int>(ec.templates.nodeFactory.size());
  currentlyLoadedPlugin = plugin;
  loadErrors = 0;
  nodesRegistered = 0;
  componentsRegistered = 0;
}

void Plugin::printLoadStats(EditorContext& ec) {
  currentlyLoadedPlugin = nullptr;
  componentsRegistered = static_cast<int>(ec.templates.componentFactory.size()) - previousCompSize;
  nodesRegistered = static_cast<int>(ec.templates.nodeFactory.size()) - previousNodeSize;

  constexpr int numBuffSize = 4;
  char numBuff[numBuffSize];
  PadWithChar(numBuff, numBuffSize, String::formatText("%d", nodesRegistered), '.');
  printf("Nodes: %s ", numBuff);

  PadWithChar(numBuff, numBuffSize, String::formatText("%d", componentsRegistered), '.');
  printf("Components: %s ", numBuff);

  PadWithChar(numBuff, numBuffSize, String::formatText("%d", loadErrors), '.');
  printf("Errors: %s", numBuff);

  printf("\n");
}

bool Plugin::loadPlugins(EditorContext& ec) {
  const char* basePath = String::formatText("%s%s", ec.string.applicationDir, String::PLUGIN_PATH);
  const char* filter;
#if defined(_WIN32)
  filter = ".dll";
#else
  filter = ".so";
#endif
  printf("Loading raynodes plugins:\n");
  for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
    if (entry.path().extension() == filter) {
      auto* nameBuff = new char[MAX_NAME_LEN];
      auto fileName = entry.path().filename().generic_string();
      strncpy_s(nameBuff, MAX_NAME_LEN, fileName.c_str(), fileName.size());
      auto* plugin = PluginLoader::GetPluginInstance(absolute(entry.path()).string().c_str(), "CreatePlugin");
      if (!plugin) {
        PadWithChar(nameBuff, MAX_NAME_LEN, fileName.c_str(), '.');
        fprintf(stderr, "Failed to load plugin: %s\n", nameBuff);
        continue;
      }
      plugin->name = nameBuff;
      plugin->onLoad(ec);
      plugins.push_back(plugin);
    }
  }

  //TODO Sort plugins before loading - so builtins come first

  for (auto* p : plugins) {
    startLoad(ec, p);
    p->registerComponents(ec);
    p->registerNodes(ec);
    printLoadStats(ec);
  }

  printf("=============================================================\n");
  printf("Loaded %d Plugin(s)\n", static_cast<int>(plugins.size()));

  return true;
}

//C-Style approach using raylib - not tested
/*
*const char* basePath = String::formatText("%s%s", ec.string.applicationDir, PLUGIN_PATH);
  const char* filter;
#if defined(_WIN32)
  filter = ".dll";
#else
  filter = ".so";
#endif

  dirent* dp = nullptr;
  DIR* dir = opendir(basePath);

  std::memset(String::buffer, 0, String::BUFFER_SIZE);

  if (dir != nullptr) {
    while ((dp = readdir(dir)) != nullptr) {
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
#if defined(_WIN32)
        sprintf(String::buffer, "%s\\%s", basePath, dp->d_name);
#else
        sprintf(path, "%s/%s", basePath, dp->d_name);
#endif

        if (IsFileExtension(String::buffer, filter)) {
          auto* plugin = new Plugin();
          if (plugin->load(String::buffer)) {
            auto* raynodePlugin = plugin->getFunction<RaynodePluginI*>("GetPlugin");
            plugins.push_back(raynodePlugin);
          }
        }
      }
    }
  }
  closedir(dir);
 */