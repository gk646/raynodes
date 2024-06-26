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

bool ComponentRegister::registerComponent(const char* id, const ComponentCreateFunc func) {
  const auto res = ec.templates.registerComponent(id, func, pc);
  if (!res) errorCount++;
  return res;
}

NodeCreateFunc NodeRegister::GetDefaultCreateFunc() {
  return [](const NodeTemplate& nt, const Vec2 pos, const NodeID id) -> Node* {
    return new Node(nt, pos, id);
  };
}

NodeTemplate NodeRegister::CreateTemplate(const char* name, const ComponentDefinition& comps, Color4 c) {
  NodeTemplate nt;
  nt.label = name;
  nt.color = c;

  int i = 0;
  for (const auto component : comps) {
    nt.components[i] = component;
    i++;
  }
  return nt;
}

bool NodeRegister::registerNode(const NodeTemplate& nt, NodeCreateFunc nodeCreateFunc) {
  const auto res = ec.templates.registerNode(nt, nodeCreateFunc, pc);

  if (res) {
    ec.ui.nodeCreateMenu.addEntry(pc.name, nt.label);
  } else {
    errorCount++;
  }
  return res;
}