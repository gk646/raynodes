#ifndef RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_

namespace Editor {
inline void PollControls(EditorContext& ec) {
  const auto mouse = ec.logic.mouse;
  const auto worldMouse = ec.logic.worldMouse;
  auto& dragStart = ec.logic.dragStart;
  auto& camera = ec.display.camera;
  auto& isDragging = ec.logic.isDraggingScreen;
  auto& selectRect = ec.logic.selectRect;
  auto& contextMenuPos = ec.logic.contextMenuPos;

  //Context menu
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {

    contextMenuPos = mouse;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    if (mouse.x == contextMenuPos.x && mouse.y == contextMenuPos.y) {
      ec.logic.showContextMenu = !ec.logic.showContextMenu;
      contextMenuPos.x -= 3;
      contextMenuPos.y -= 3;
      ec.logic.isDraggingScreen = false;
      ec.logic.isSelecting = false;
      return;
    }
  }

  //Panning
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ec.logic.isAnyNodeHovered) {
    ec.logic.isSelecting = false;
    ec.core.selectedNodes.clear();
    dragStart = worldMouse;
    isDragging = true;
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isDragging = false;
  }
  if (isDragging) {
    camera.target.x -= (worldMouse.x - dragStart.x);
    camera.target.y -= (worldMouse.y - dragStart.y);
    dragStart = GetScreenToWorld2D(ec.logic.mouse, camera);
  }

  //Selecting
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !ec.logic.isAnyNodeHovered && !ec.logic.showContextMenu) {
    ec.logic.isSelecting = true;
    selectRect.x = worldMouse.x;
    selectRect.y = worldMouse.y;
    selectRect.width = 0;
    selectRect.height = 0;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    ec.logic.isSelecting = false;
  }

  //Shortcuts
  if (IsKeyPressed(KEY_DELETE)) {
    for (auto pair : ec.core.selectedNodes) {
      ec.core.removeNode(NodeID(pair.first));
      delete pair.second;
    }
    ec.core.selectedNodes.clear();
  }

  //Undo
  if (IsKeyPressed(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) {

    ec.core.undo();
  }

  //Redo
  if (IsKeyPressed(KEY_Y) && IsKeyDown(KEY_LEFT_CONTROL)) {
    ec.core.redo();
  }

  if (IsKeyDown(KEY_B)) {
    ec.core.createNode(NodeType::HEADER, {(float)GetRandomValue(0, 150), 500});
    printf("%d\n", (int)ec.core.UID);
  }
}

}  // namespace Editor

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
