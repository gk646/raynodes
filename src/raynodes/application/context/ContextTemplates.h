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
    std::unordered_map<const char*, NodeCreateFunc, cxstructs::Fnv1aHash, cxstructs::StrEqual>;

struct NodeTemplate {
  cxstructs::StackVector<const char*, 10> components;  //Component names are unique
};

struct Template {
  StringComponentMap componentFactory;
  StringNodeMap nodeFactory;
  std::vector<NodeTemplate> templates;

  //Passed name only has to be valid until this function returns (copied)
  bool registerComponent(const char* name, ComponentCreateFunc func) {
    if (componentFactory.contains(name)) {
      fprintf(stderr, "Colliding component \"%s\" will not be loaded. Please contact the plugin author.");
      return false;
    }
    auto* copy = new char[strlen(name) + 1];
    strcpy(copy, name);
    componentFactory.insert({copy, func});
    return true;
  }
  //Passed name only has to be valid until this function returns (copied)
  Component* createComponent(const char* name) {
    const auto it = componentFactory.find(name);
    if (it != componentFactory.end()) {
      return it->second(it->first);  //Reuse the allocated name
    }
    return nullptr;
  }
  //Passed name only has to be valid until this function returns (copied)
  bool registerNode(const char* name, NodeCreateFunc func) {
    if (nodeFactory.contains(name)) {
      fprintf(stderr, "Colliding component \"%s\" will not be loaded. Please contact the plugin author.");
      return false;
    }
    auto* copy = new char[strlen(name) + 1];
    strcpy(copy, name);
    nodeFactory.insert({copy, func});
    return true;
  }
  //Passed name only has to be valid until this function returns (copied)
  Node* createNode(const char* name) {
    const auto it = nodeFactory.find(name);
    if (it != nodeFactory.end()) {
      return it->second(it->first);  //Reuse the allocated name
    }
    return nullptr;
  }
};

// Special interfaces to capsulate the registration
struct ComponentRegister {
  EditorContext& ec;
  bool registerComponent(const char* name, ComponentCreateFunc func);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTTEMPLATES_H_