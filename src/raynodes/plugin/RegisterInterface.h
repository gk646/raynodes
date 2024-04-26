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

#ifndef REGISTERINTERFACE_H
#define REGISTERINTERFACE_H

#include <initializer_list>

#include "shared/fwd.h"

// Special interfaces to capsulate the registration
struct ComponentRegister {
  EditorContext& ec;
  RaynodesPluginI& plugin;
  int errorCount = 0;

  // Simplest method to register a component - just a name and the type
  template <typename CompType>
  bool registerComponent(const char* name) {
    return registerComponent(name, GetCreateFunc<CompType>());
  }

 private:
  // Register a component to be used with the given name
  // "ComponentCreateFunc" is just a function that takes a string name and returns a "Component*"
  bool registerComponent(const char* name, ComponentCreateFunc func);

  // Useful when registering components e.g: cr.registerComponent("MathOp", GetCreateFunc<MathC>());
  template <typename T>
  static ComponentCreateFunc GetCreateFunc() {
    return [](const ComponentTemplate ct) -> Component* {
      return new T(ct);
    };
  }
};

struct NodeRegister {
  EditorContext& ec;
  RaynodesPluginI& plugin;
  int errorCount = 0;
  // Simplest method to register a node with a "name" and the components specified directly
  bool registerNode(const char* name, const std::initializer_list<ComponentTemplate>& components);

 private:
  // Registers a node with name "name" with the components specified in the template
  bool registerNode(const char* name, const NodeTemplate& nt);
};

#endif  //REGISTERINTERFACE_H