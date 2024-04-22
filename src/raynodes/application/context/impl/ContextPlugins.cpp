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

#include "application/EditorContext.h"
#include "plugin/Plugin.h"

String::String() {
  applicationDir = GetApplicationDirectory();
}

bool Plugins::loadPlugins(EditorContext& ec) {
  const char* basePath = String::formatText("%s%s", ec.string.applicationDir, PLUGIN_PATH);
  const char* filter;
#if defined(_WIN32)
  filter = ".dll";
#else
  filter = ".so";
#endif

  for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
    if (entry.path().extension() == filter) {
      std::string absolutePath = absolute(entry.path()).string();
      auto* plugin = Plugin::loadPlugin(absolutePath.c_str(), "CreatePlugin");
      plugin->onLoad(ec);
      plugins.push_back(plugin);
    }
  }

  for (auto* p : plugins) {
    p->registerComponents(ec);
    p->registerNodes(ec);
  }

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