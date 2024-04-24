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

#ifndef CONTEXTPLUGIN_H
#define CONTEXTPLUGIN_H

struct Plugin {
  static constexpr int MAX_NAME_LEN = 17;  //16 with terminator ('\0')
  std::vector<RaynodesPluginI*> plugins;
  const RaynodesPluginI* currentlyLoadedPlugin = nullptr;
  int previousCompSize = 0;
  int previousNodeSize = 0;
  int loadErrors = 0;
  int componentsRegistered = 0;
  int nodesRegistered = 0;

  void startLoad(EditorContext& ec, const RaynodesPluginI* plugin);
  void printLoadStats(EditorContext& ec);
  bool loadPlugins(EditorContext& ec);
};
#endif  //CONTEXTPLUGIN_H