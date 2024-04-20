#ifndef RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
#include <ranges>

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
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    contextMenuPos = mouse;
  }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
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
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ec.logic.isAnyNodeHovered) {
    ec.logic.isSelecting = false;
    ec.core.selectedNodes.clear();
    dragStart = worldMouse;
    isDragging = true;
  }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    ec.logic.handleDroppedPin(ec);
    auto& moveAction = ec.logic.currentMoveAction;
    if (moveAction != nullptr) {
      const auto avgDist = moveAction->calculateDeltas(ec);
      if (avgDist < Logic::MIN_DIST_THRESHOLD) {
        delete moveAction;
      } else {
        ec.core.addEditorAction(moveAction);
      }
      moveAction = nullptr;
    }
    isDragging = false;
  }
  if (isDragging) {
    camera.target.x -= (worldMouse.x - dragStart.x);
    camera.target.y -= (worldMouse.y - dragStart.y);
    dragStart = GetScreenToWorld2D(ec.logic.mouse, camera);
  }

  //Selecting
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !ec.logic.isAnyNodeHovered
      && !ec.logic.showContextMenu) {
    ec.core.selectedNodes.clear();
    ec.logic.isSelecting = true;
    ec.logic.selectPoint = worldMouse;
    selectRect.width = 0;
    selectRect.height = 0;
  }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    ec.logic.isSelecting = false;
  }

  //Delete
  if (ec.input.isKeyPressed(KEY_DELETE)) {
    //Skip if empty
    if (!ec.core.selectedNodes.empty()) {
      const auto action = new NodeDeleteAction(ec, ec.core.selectedNodes);
      ec.core.addEditorAction(action);
      ec.core.selectedNodes.clear();
    }
  }

  //My keyboard...
  //Redo
  if ((ec.input.isKeyPressed(KEY_Z) || ec.input.isKeyPressedRepeat(KEY_Z))
      && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    ec.core.redo(ec);
  }

  //Undo
  if ((ec.input.isKeyPressed(KEY_Y) || ec.input.isKeyPressedRepeat(KEY_Y))
      && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    ec.core.undo(ec);
  }

  //CTRL + (V,C,X) shortcuts
  if (ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    auto& copiedNodes = ec.core.copiedNodes;
    if (ec.input.isKeyPressed(KEY_C)) {
      copiedNodes.clear();
      for (const auto& node : ec.core.selectedNodes | std::views::values) {
        copiedNodes.push_back(node);
      }
    } else if (ec.input.isKeyPressed(KEY_X)) {
      copiedNodes.clear();
      for (const auto& [id, node] : ec.core.selectedNodes) {
        copiedNodes.push_back(node);
        ec.core.removeNode(ec, id);
      }
    } else if (ec.input.isKeyPressed(KEY_V)) {
      for (const auto n : copiedNodes) {
        const auto newNode = n->clone(ec.core.getNextID());
        ec.core.insertNode(ec, newNode->id, newNode);
      }
      copiedNodes.clear();
    }
  }

  if (ec.input.isKeyDown(KEY_B)) {
    ec.core.createNode(ec, NodeType::HEADER,
                       {(float)GetRandomValue(0, 1000), (float)GetRandomValue(0, 1000)});
    printf("%d\n", static_cast<int>(ec.core.nodes.size()));
  }
}

}  // namespace Editor

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_