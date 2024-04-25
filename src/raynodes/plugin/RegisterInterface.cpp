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

#include "application/EditorContext.h"
#include "plugin/PluginInterface.h"

bool ComponentRegister::registerComponent(const char* name, ComponentCreateFunc func) {
  const auto res = ec.templates.registerComponent(name, func, plugin);
  if (!res) errorCount++;
  return res;
}

bool NodeRegister::registerNode(const char* name, NodeTemplate& nt) {
  const auto res = ec.templates.registerNode(name, nt, plugin);
  if (res) {
    ec.ui.contextMenu.addNode(plugin.name, name);
  } else {
    errorCount++;
  }
  return res;
}

bool NodeRegister::registerNode(const char* name, const std::initializer_list<const char*>& components) {
  NodeTemplate nt;
  int i = 0;
  for (const auto component : components) {
    nt.components[i] = component;
    i++;
  }
  const auto res = ec.templates.registerNode(name, nt, plugin);
  if (res) {
    ec.ui.contextMenu.addNode(plugin.name, name);
  } else {
    errorCount++;
  }
  return res;
}