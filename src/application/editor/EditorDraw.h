#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_

#include "nodes/Nodes.h"

namespace Editor {

inline void DrawBackGround(EditorContext& ec) {
  Editor::StartUpdateTick(ec);
  Editor::DrawGrid(ec);
}

inline void DrawContent(EditorContext& ec) {
  auto& nodes = ec.core.nodes;
  auto& selectRect = ec.logic.selectRect;
  const auto& camera = ec.display.camera;

  UpdateResource ur = ec.getUpdateResource();
  DrawResource dr = ec.getDrawResource();

  for (auto n : nodes) {
    n->update(ur);
    n->draw(dr);
  }

  ec.logic.isAnyNodeHovered = ur.anyNodeHovered;
  ec.logic.isDraggingNode = ur.isDraggingNode;

  if (ur.isSelecting) {
    selectRect.width = ur.worldMouse.x - selectRect.x;
    selectRect.height = ur.worldMouse.y - selectRect.y;
    DrawRectanglePro(selectRect, {0, 0}, 0, ColorAlpha(BLUE, 0.4F));
  }
}

inline void DrawForeGround(EditorContext& ec) {
  if (ec.logic.showContextMenu) {
    Editor::DrawContextMenu(ec);
  }
  Editor::PollControls(ec);

  char buff[8];
  snprintf(buff, 8, "%d", GetFPS());
  DrawTextEx(ec.display.editorFont, buff, {25, 25}, 16, 1.0F, GREEN);
}

}  // namespace Editor
#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
