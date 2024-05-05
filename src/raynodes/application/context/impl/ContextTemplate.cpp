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

bool Template::registerComponent(const char* name, ComponentCreateFunc func, const RaynodesPluginI& plugin) {
  if (componentFactory.contains(name)) {
    auto* format = "Naming collision: %s/%s wont be loaded. Please contact the plugin author.\n";
    fprintf(stderr, format, plugin.name, name);
    return false;
  }
  componentFactory.insert({cxstructs::str_dup(name), func});
  return true;
}

bool Template::registerNode(const NodeTemplate& nt, NodeCreateFunc func, const RaynodesPluginI& plugin) {
  if (nodeFactory.contains(nt.label)) {
    auto* format = "Naming collision: %s/%s wont be loaded. Please contact the plugin author.\n";
    fprintf(stderr, format, plugin.name, nt.label);
    return false;
  }
  // Deep copy the template
  NodeTemplate newTemplate;
  newTemplate.color = nt.color;
  newTemplate.label = cxstructs::str_dup(nt.label);
  for (int i = 0; i < COMPS_PER_NODE; ++i) {
    if (nt.components[i].component == nullptr) continue;  // We dont break for safety
    newTemplate.components[i].component = cxstructs::str_dup(nt.components[i].component);
    newTemplate.components[i].label = cxstructs::str_dup(nt.components[i].label);
  }

  nodeTemplates.insert({newTemplate.label, newTemplate});
  nodeFactory.insert({newTemplate.label, func});
  return true;
}