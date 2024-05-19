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
#include "cxstructs/StackVector.h"

struct ContextActionInfo {
  NodeContextAction action;
  const char* name;
  uint8_t iconID;
};

struct NodeContextMenu {
  static constexpr int SIZE = 4;
  cxstructs::StackVector<ContextActionInfo, SIZE> actions;
  cxstructs::StackVector<ContextActionInfo, SIZE> quickActions;

  void draw(EditorContext& ec, const Vector2& pos);
  void registerAction(const char* name, NodeContextAction action, uint8_t iconID) {
    for (const auto& n : actions) {
      if (strcmp(n.name, name) == 0) {
        fprintf(stderr, "Action with this name already exists");
        return;
      }
    }
    actions.push_back({action, name, iconID});
  }
  void registerQickAction(const char* name, NodeContextAction action, uint8_t iconID) {
    for (const auto& n : quickActions) {
      if (strcmp(n.name, name) == 0) {
        fprintf(stderr, "QuickAction with this name already exists");
        return;
      }
    }
    quickActions.push_back({action, name, iconID});
  }

 private:
  void drawQuickActions();
};

#endif  //ACTIONMENU_H