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

bool Template::registerComponent(const char* name, ComponentCreateFunc func, const PluginContainer& pc) {
  if (componentFactory.contains(name)) {
    auto* format = "Naming collision: %s/%s wont be loaded. Please contact the plugin author.\n";
    fprintf(stderr, format, pc.name, name);
    return false;
  }
  componentFactory.insert({cxstructs::str_dup(name), func});
  return true;
}

bool Template::registerNode(const NodeTemplate& nt, NodeCreateFunc func, const PluginContainer& pc) {
  if (registeredNodes.contains(nt.label)) {
    auto* format = "Name Collision Error: Node already exists with name: %s";
    fprintf(stderr, format, nt.label);
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

    // Checks
    if (!componentFactory.contains(newTemplate.components[i].component)) {
      auto* format = "Illegal Component Error: %s/%s tries to register with missing component: %s\n";
      fprintf(stderr, format, pc.name, nt.label, newTemplate.components[i].component);
      return false;
    }

    for (int j = 0; j < COMPS_PER_NODE; ++j) {
      if (j == i || nt.components[i].label == nullptr || nt.components[j].label == nullptr) continue;
      if (strcmp(nt.components[i].label, nt.components[j].label) == 0) {
        auto* format = "Label Duplicate Warning: Using unique component labels is advised: %s/%s/%s\n";
        fprintf(stderr, format,pc.name,nt.label, nt.components[i].label);
      }
    }
  }

  registeredNodes.insert({newTemplate.label, NodeInfo{newTemplate, func}});
  return true;
}
Node* Template::createNode(EditorContext& ec, const char* name, Vec2 pos, NodeID nodeID) {
  NodeInfo* info = nullptr;

  if (registeredNodes.contains(name)) [[likely]] {
    info = &registeredNodes[name];
  } else if (userDefinedNodes.contains(name)) [[likely]] {
    info = &userDefinedNodes[name];
  } else {
    fprintf(stderr, "No node registered with the name %s", name);
    return nullptr;
  }

  if (info == nullptr) return nullptr;  // Safety

  auto* node = info->createFunc(info->nTemplate, pos, nodeID);  // Has to exist
  for (const auto component : info->nTemplate.components) {
    if (component.component == nullptr) continue;  // We dont break for safety
    auto* comp = createComponent(component);
    if (comp) node->components.push_back(comp);
    else fprintf(stderr, "No component registered with the name %s", component.component);
  }

  // Call event functions
  for (const auto c : node->components) {
    c->onCreate(ec, *node);
  }

  node->onCreation(ec);  // Called after all components

  return node;
}

bool Template::registerUserNode(EditorContext& /**/, const NodeTemplate& nt, NodeCreateFunc func) {

  userDefinedNodes.insert({nt.label, {nt, func}});
  return true;
}