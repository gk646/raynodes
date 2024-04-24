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

#ifndef PLUGINI_H
#define PLUGINI_H

#include "shared/fwd.h"

using RegisterCompFunc = bool (*)(const char*, ComponentCreateFunc);

struct RaynodesPluginI {
  const char* name = nullptr;  //Will be set automatically at runtime (to the file name, for clarity)
  virtual ~RaynodesPluginI() = default;
  virtual void onLoad(EditorContext& ec) {}
  virtual void registerComponents(ComponentRegister& cr) {}
  virtual void registerNodes(EditorContext& ec) {}

 protected:
  // Useful when registering components e.g: cr.registerComponent("MathOp", GetCreateFunc<MathC>());
  template <typename T>
  static auto GetCreateFunc() {
    return [](const char* componentName) -> Component* {
      return new T(componentName);
    };
  }
};

// IMPORTANT: Include this at the bottom
/*
extern "C" EXPORT inline RaynodesPluginI* CreatePlugin() {
  return new MyPlugin();
}
*/
#endif  //PLUGINI_H