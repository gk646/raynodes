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

#include <cstdio>
#include <raylib.h>
#include <cxutil/cxstring.h>

#include "shared/fwd.h"

#include "plugin/PluginInterface.h"
#include "blocks/Connection.h"

#include "context/ContextString.h"
#include "context/ContextInfo.h"
#include "context/ContextCore.h"
#include "context/ContextDisplay.h"
#include "context/ContextLogic.h"
#include "context/ContextPersist.h"
#include "context/ContextInput.h"
#include "context/ContextTemplates.h"
#include "context/ContextPlugin.h"

struct EditorContext {
  String string{};
  Core core{};
  Display display{};
  Logic logic{};
  Templates templates{};
  Persist persist{};
  Input input{};
  Info info{};
  Plugin plugin{};

  explicit EditorContext(const char* openedFile) {
    persist.openedFile = openedFile;
    printf("raynodes - Version %d.%d\n", Info::majorVersion, Info::minorVersion);
  }
};
#endif  //RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_