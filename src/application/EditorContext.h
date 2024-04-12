#ifndef RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_

#include <raylib.h>

#include "shared/fwd.h"
#include "shared/Types.h"

#include "context/ContextInfo.h"
#include "context/ContextCore.h"
#include "context/ContextDisplay.h"
#include "context/ContextLogic.h"
#include "context/ContextPersist.h"

struct EditorContext
{
  Info info{};
  Display display{};
  Core core{};
  Logic logic{};
  Persist persist{};

  EditorContext(const char* openedFile) { persist.openedFile = openedFile; };
};

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
