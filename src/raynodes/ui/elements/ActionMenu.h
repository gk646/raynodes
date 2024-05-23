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

#ifndef ACTIONMENU_H
#define ACTIONMENU_H

#include "shared/fwd.h"

#include <raylib.h>
#include <vector>

template <class T>
struct ActionMenu {
  using ContextAction = void (*)(EditorContext&, T&);  // Function for the action menu

  struct ContextActionInfo {  // Info holder
    ContextAction action;
    const char* name;
    uint8_t iconID;
  };

  bool isVisible = false;
  const char* menuName = nullptr;
  std::vector<ContextActionInfo> actions;
  std::vector<ContextActionInfo> quickActions;

  explicit ActionMenu(const char* name) : menuName(name) {
    actions.reserve(5);
    quickActions.reserve(5);
  }
  void draw(EditorContext& ec, Vector2 pos);
  void registerAction(const char* name, const ContextAction action, const uint8_t iconID) {
    for (const auto& n : actions) {
      if (strcmp(n.name, name) == 0) {
        fprintf(stderr, "Action with this name already exists");
        return;
      }
    }
    actions.push_back({action, name, iconID});
  }
  void registerQickAction(const char* tooltip, ContextAction action, uint8_t iconID) {
    for (const auto& n : quickActions) {
      if (strcmp(n.name, tooltip) == 0) {
        fprintf(stderr, "QuickAction with this tooltip already exists");
        return;
      }
    }
    quickActions.push_back({action, tooltip, iconID});
  }
  void show() { isVisible = true; }
  void hide() { isVisible = false; }
};

#endif  //ACTIONMENU_H