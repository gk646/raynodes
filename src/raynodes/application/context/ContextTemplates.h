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

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_

using Comp = cxstructs::StrEqual;
using Hash = cxstructs::Fnv1aHash;

using ComponentMap = std::unordered_map<const char*, ComponentCreateFunc, Hash, Comp>;
using NodeCreateMap = std::unordered_map<const char*, NodeInfo, Hash, Comp>;

struct Template {
  ComponentMap componentFactory;
  NodeCreateMap registeredNodes;
  NodeCreateMap userDefinedNodes;

  // Passed name only has to be valid until this function returns (copied)
  bool registerComponent(const char* name, ComponentCreateFunc func, const PluginContainer& plugin);
  // "ComponentTemplate" MUST BE allocated and unchanged for the lifetime of the component
  Component* createComponent(const ComponentTemplate component) {
    const auto it = componentFactory.find(component.component);
    if (it == componentFactory.end()) [[unlikely]] { return nullptr; }
    return it->second(component);  //Reuse the allocated name
  }
  // Passed template names only have to be valid until this function returns (copied)
  bool registerNode(const NodeTemplate& nt, NodeCreateFunc func, const PluginContainer& plugin);
  // Passed name only has to be valid until this function returns
  Node* createNode(const char* name, Vec2 pos, NodeID nodeID) {
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
    return node;
  }

  bool registerUserNode(const NodeTemplate& nt, NodeCreateFunc func) {

    userDefinedNodes.insert({nt.label,{nt, func}});
    return true;
  }
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_