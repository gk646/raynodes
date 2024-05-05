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

  // Register a component with just a name and the its type
  // IMPORTANT: Name has to be unique! - errors will be shown when plugin is loaded
  template <class ComponentType>
  bool registerComponent(const char* name) {
    return registerComponent(name, GetCreateFunc<ComponentType>());
  }

 private:
  bool registerComponent(const char* name, ComponentCreateFunc func);

  // Generates the createFunction
  template <class ComponentType>
  static ComponentCreateFunc GetCreateFunc() {
    return [](const ComponentTemplate ct) -> Component* {
      return new ComponentType(ct);
    };
  }
};

struct NodeRegister {
  EditorContext& ec;
  RaynodesPluginI& plugin;
  int errorCount = 0;

  // Register a node with a "name" and the components specified directly - optional header color
  bool registerNode(const char* name, const std::initializer_list<ComponentTemplate>& components,
                    Color4 color = {0, 0, 0, 255});

  // Register a custom node by specifying its type
  template <class CustomNodeType>
  bool registerCustomNode(const char* name, const std::initializer_list<ComponentTemplate>& components,
                          Color4 color = {0, 0, 0, 255}) {
    return registerNode(CreateTemplate(name, components, color), GetCreateFunc<CustomNodeType>());
  }

 private:
  using ComponentDefinition = std::initializer_list<ComponentTemplate>;
  static NodeTemplate CreateTemplate(const char* name, const ComponentDefinition& components, Color4 color);
  // Generates the createFunction
  template <class CustomNodeType>
  static NodeCreateFunc GetCreateFunc() {
    return [](const NodeTemplate& nt) -> Node* {
      return new CustomNodeType(nt);
    };
  }
  // Registers a node with name "name" with the components specified in the template
  bool registerNode(const NodeTemplate& nt, NodeCreateFunc nodeCreateFunc);
};

#endif  //REGISTERINTERFACE_H