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

struct NodeTemplate;
using StringComponentMap =
    std::unordered_map<const char*, ComponentCreateFunc, cxstructs::Fnv1aHash, cxstructs::StrEqual>;
using StringNodeMap =
    std::unordered_map<const char*, NodeTemplate, cxstructs::Fnv1aHash, cxstructs::StrEqual>;

struct NodeTemplate {
  const char* components[10] = {nullptr};  //Component names are unique
};

struct Template {
  StringComponentMap componentFactory;
  StringNodeMap nodeFactory;

  // Passed name only has to be valid until this function returns (copied)
  bool registerComponent(const char* name, ComponentCreateFunc func, const RaynodesPluginI& plugin) {
    if (componentFactory.contains(name)) {
      auto* format = "Naming collision: %s/%s wont be loaded. Please contact the plugin author.\n";
      fprintf(stderr, format, plugin.name, name);
      return false;
    }
    componentFactory.insert({cxstructs::str_dup(name), func});
    return true;
  }
  // Passed name only has to be valid until this function returns
  Component* createComponent(const char* name) {
    const auto it = componentFactory.find(name);
    if (it != componentFactory.end()) {
      return it->second(it->first);  //Reuse the allocated name
    }
    return nullptr;
  }
  // Passed template names only have to be valid until this function returns (copied)
  bool registerNode(const char* name, const NodeTemplate& nt, const RaynodesPluginI& plugin) {
    if (nodeFactory.contains(name)) {
      auto* format = "Naming collision: %s/%s wont be loaded. Please contact the plugin author.\n";
      fprintf(stderr, format, plugin.name, name);
      return false;
    }
    NodeTemplate newTemplate;  // Allocate and copy the given component names
    for (int i = 0; i < 10; ++i) {
      if (nt.components[i] == nullptr) continue;  // We dont break for safety
      newTemplate.components[i] = cxstructs::str_dup(nt.components[i]);
    }

    nodeFactory.insert({cxstructs::str_dup(name), newTemplate});  // Allocate and copy the name
    return true;
  }
  // Passed name only has to be valid until this function returns
  Node* createNode(const char* name) {
    const auto it = nodeFactory.find(name);
    if (it != nodeFactory.end()) {
      auto* node = new Node(it->first);
      for (const auto component : it->second.components) {
        if (component == nullptr) continue;  // We dont break for safety
        node->components.push_back(createComponent(component));
      }
      return node;
    }
    return nullptr;
  }
};

// Special interfaces to capsulate the registration
struct ComponentRegister {
  EditorContext& ec;
  RaynodesPluginI& plugin;
  int errorCount = 0;
  // Register a component to be used with the given name
  // "ComponentCreateFunc" is just a function that takes a string name and returns a"Component*"
  bool registerComponent(const char* name, ComponentCreateFunc func);
};
struct NodeRegister {
  EditorContext& ec;
  RaynodesPluginI& plugin;
  int errorCount = 0;
  // Registers a node with name "name" with the components specified in the template
  bool registerNode(const char* name, NodeTemplate& nt);
  // Registers a node with name "name" and the component names specified directly
  bool registerNode(const char* name, const std::initializer_list<const char*>& components);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_