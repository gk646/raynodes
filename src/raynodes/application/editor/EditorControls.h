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

#ifndef RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_

namespace Editor {
//TODO split up

inline void PollControls(EditorContext& ec) {
  const auto mouse = ec.logic.mouse;
  const auto worldMouse = ec.logic.worldMouse;
  auto& dragStart = ec.logic.dragStart;
  auto& camera = ec.display.camera;
  auto& isDragging = ec.logic.isDraggingScreen;
  auto& selectRect = ec.logic.selectRect;
  auto& selectedNodes = ec.core.selectedNodes;
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
    selectedNodes.clear();
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
        ec.core.addEditorAction(ec, moveAction);
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
    selectedNodes.clear();
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
    if (!selectedNodes.empty()) {
      const auto action = new NodeDeleteAction(ec, selectedNodes);
      ec.core.addEditorAction(ec, action);
      selectedNodes.clear();
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

  //CTRL + (V,C,X,+,-,S) shortcuts
  if (ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    auto& copiedNodes = ec.core.copiedNodes;
    if (ec.input.isKeyPressed(KEY_C) && !selectedNodes.empty()) {
      copiedNodes.clear();
      for (const auto& node : selectedNodes | std::views::values) {
        copiedNodes.push_back(node);
      }
    } else if (ec.input.isKeyPressed(KEY_X) && !selectedNodes.empty()) {
      copiedNodes.clear();
      for (const auto n : selectedNodes | std::views::values) {
        copiedNodes.push_back(n);
      }
      const auto action = new NodeDeleteAction(ec, selectedNodes);
      ec.core.addEditorAction(ec, action);
      selectedNodes.clear();
    } else if (ec.input.isKeyPressed(KEY_V) && !copiedNodes.empty()) {
      Vector2 delta = {worldMouse.x - copiedNodes[0]->position.x, worldMouse.y - copiedNodes[0]->position.y};
      const auto action = new NodeCreateAction(static_cast<int>(copiedNodes.size()) + 1);
      for (const auto n : copiedNodes) {
        const auto newNode = n->clone(ec.core.getNextID());
        newNode->position.x += delta.x;
        newNode->position.y += delta.y;
        action->createdNodes.push_back(newNode);
        ec.core.insertNode(ec, newNode->id, newNode);
      }
      ec.core.addEditorAction(ec, action);
    } else if (ec.input.isKeyPressed(KEY_S)) {
      ec.persist.saveToFile(ec);
      //TODO add dialogue if not name given
    } else if (ec.input.isKeyPressed(KEY_SLASH)) {
      ec.display.zoomOut();
    } else if (ec.input.isKeyPressed(KEY_RIGHT_BRACKET)) {
      ec.display.zoomIn();
    }
  }

  //TODO remove debug
  // printf("%d\n", GetKeyPressed());
  if (ec.input.isKeyDown(KEY_B)) {
    ec.core.createNode(ec, "Display", {(float)GetRandomValue(0, 1000), (float)GetRandomValue(0, 1000)});
    printf("%d\n", static_cast<int>(ec.core.nodes.size()));
  }
}

}  // namespace Editor

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_