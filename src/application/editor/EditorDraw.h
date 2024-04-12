#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_

#include "nodes/Nodes.h"

namespace Editor {

inline void DrawBackGround(EditorContext& ec) {
  Editor::DrawGrid(ec);
}

inline void DrawContent(EditorContext& ec) {
  auto& nodes = ec.core.nodes;

  //Critical section
  ec.core.lock();
  {
    for (auto n : nodes)
      {
        n->draw(ec);
      }
  }
  ec.core.unlock();

  if (ec.logic.isSelecting)
    {
      DrawRectanglePro(ec.logic.selectRect, {0, 0}, 0, ColorAlpha(BLUE, 0.4F));
    }
}

inline void DrawForeGround(EditorContext& ec) {
  if (ec.logic.showContextMenu)
    {
      Editor::DrawContextMenu(ec);
    }

  Editor::DrawActions(ec);

  //Critical section // (Undo / Redo )
  ec.core.lock();
  { Editor::PollControls(ec); }
  ec.core.unlock();

  char buff[8];
  snprintf(buff, 8, "%d", GetFPS());
  DrawTextEx(ec.display.editorFont, buff, {25, 25}, 16, 1.0F, GREEN);
}

}  // namespace Editor
#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
