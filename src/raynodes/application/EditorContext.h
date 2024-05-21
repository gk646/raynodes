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

#ifndef RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_

#include "shared/fwd.h"

#include <cstdio>
#include <cmath>

#pragma warning(push)
#pragma warning(disable : 4251)  // Remove export warning

#include <deque>
#include <unordered_map>
#include <vector>
#include <string>

#include <raylib.h>
#include <cxutil/cxstring.h>

#include "ui/elements/CanvasContextMenu.h"
#include "ui/elements/NodeContextMenu.h"

#include "blocks/Connection.h"
#include "blocks/NodeGroup.h"
#include "node/Node.h"

#include "context/ContextInfo.h"
#include "context/ContextString.h"
#include "context/ContextCore.h"
#include "context/ContextUI.h"
#include "context/ContextDisplay.h"
#include "context/ContextLogic.h"
#include "context/ContextPersist.h"
#include "context/ContextInput.h"
#include "context/ContextTemplates.h"
#include "context/ContextPlugin.h"

// We actually wanna keep this as small as possible
// So its always hot in cache
// Sorted after access pattern and size - sadly cant use perf with cache misses on WSL

struct EXPORT EditorContext {
  Input input{};
  Core core{};
  Logic logic{};
  UI ui{};
  Display display{};
  String string{};
  Template templates{};
  Plugin plugin{};
  Persist persist{};
  Info info{};

  explicit EditorContext(int argc, char* argv[]) {
    if (argc == 2) {
      if (*argv[1] == '.') {  // Relative path
        persist.openedFilePath = string.formatText("%s%s", string.applicationDir, argv[1]);
      } else {
        persist.openedFilePath = argv[1];
      }
    }

    printf("raynodes - Version %s\n", Info::getVersion(*this));
  }
};

#pragma warning(pop)

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_