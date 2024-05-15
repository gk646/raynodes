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

template <int N>
struct RegisterHelper {
  static_assert(N - 1 < PLG_MAX_NAME_LEN, "Name is too long!");
  static bool registerNode(const char (&id)[N], NodeRegister& nr);
};

// Special interfaces to capsulate the registration
struct EXPORT ComponentRegister {
  // Register a component with just a name and the its type
  // IMPORTANT: Name has to be unique! - errors will be shown when plugin is loaded
  template <typename ComponentType, int N>
  constexpr bool registerComponent(const char (&id)[N]) {
    static_assert(N - 1 < PLG_MAX_NAME_LEN, "Name is too long!");
    return registerComponent(id, GetCreateFunc<ComponentType>());
  }

  ComponentRegister(EditorContext& ec, PluginContainer& pc) : ec(ec), pc(pc), errorCount(0) {}
  [[nodiscard]] int getErrors() const { return errorCount; }

 private:
  bool registerComponent(const char* id, ComponentCreateFunc func);

  // Generates the createFunction
  template <class ComponentType>
  static ComponentCreateFunc GetCreateFunc() {
    return [](const ComponentTemplate ct) -> Component* {
      return new ComponentType(ct);
    };
  }

  EditorContext& ec;
  PluginContainer& pc;
  int errorCount;
};

struct EXPORT NodeRegister {
  // Register a node with a "name" and the components specified directly - optional header color
  template <int N>
  bool registerNode(const char (&id)[N], const std::initializer_list<ComponentTemplate>& components,
                    Color4 color = {0, 0, 0, 255}) {
    static_assert(N - 1 < PLG_MAX_NAME_LEN, "Name is too long!");
    return registerNode(CreateTemplate(id, components, color), GetDefaultCreateFunc());
  }

  // Register a custom node by specifying its type
  template <class CustomNodeType, int N>
  bool registerCustomNode(const char (&id)[N], const std::initializer_list<ComponentTemplate>& components,
                          Color4 color = {0, 0, 0, 255}) {
    static_assert(N - 1 < PLG_MAX_NAME_LEN, "Name is too long!");
    return registerNode(CreateTemplate(id, components, color), GetCreateFunc<CustomNodeType>());
  }

  NodeRegister(EditorContext& ec, PluginContainer& pc) : ec(ec), pc(pc), errorCount(0) {}
  [[nodiscard]] int getErrors() const { return errorCount; }

 private:
  using ComponentDefinition = std::initializer_list<ComponentTemplate>;
  static NodeTemplate CreateTemplate(const char* name, const ComponentDefinition& components, Color4 color);
  // Generates the createFunction
  template <class CustomNodeType>
  static NodeCreateFunc GetCreateFunc() {
    return [](const NodeTemplate& nt, Vec2 pos, NodeID id) -> Node* {
      return new CustomNodeType(nt, pos, id);
    };
  }
  static NodeCreateFunc GetDefaultCreateFunc();
  // Registers a node with name "name" with the components specified in the template
  bool registerNode(const NodeTemplate& nt, NodeCreateFunc nodeCreateFunc);

  EditorContext& ec;
  PluginContainer& pc;
  int errorCount;
};

#endif  //REGISTERINTERFACE_H