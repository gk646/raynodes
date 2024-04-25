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
  bool registerComponent(const char* name, ComponentCreateFunc func, const RaynodesPluginI& plugin);
  // Passed name only has to be valid until this function returns
  Component* createComponent(const char* name) {
    const auto it = componentFactory.find(name);
    if (it != componentFactory.end()) {
      return it->second(it->first);  //Reuse the allocated name
    }
    return nullptr;
  }
  // Passed template names only have to be valid until this function returns (copied)
  bool registerNode(const char* name, const NodeTemplate& nt, const RaynodesPluginI& plugin);
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

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_