#ifndef RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_

#include <raylib.h>

#include "shared/fwd.h"

#include "context/ContextInfo.h"
#include "context/ContextCore.h"
#include "context/ContextDisplay.h"
#include "context/ContextLogic.h"
#include "context/ContextPersist.h"
#include "shared/Types.h"

struct EditorContext {
  Info info{};
  Display display{};
  Core core{};
  Logic logic{};
  Persist persist{};

  EditorContext(const char* openedFile) { persist.openedFile = openedFile; };

  DrawResource getDrawResource() {
    DrawResource dr;
    dr.font = display.editorFont;
    dr.fontSize = display.fontSize;
    dr.worldMouse = GetScreenToWorld2D(logic.mouse, display.camera);
    return dr;
  }

  UpdateResource getUpdateResource() {
    UpdateResource ur;
    ur.worldMouse = GetScreenToWorld2D(logic.mouse, display.camera);
    ur.selectedNodes = &core.selectedNodes;
    ur.selectRect = logic.selectRect;
    ur.isSelecting = logic.isSelecting;
    ur.anyNodeHovered = false;  //reset each cycle
    ur.isDraggingNode = logic.isDraggingNode;
    return ur;
  }
};

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
