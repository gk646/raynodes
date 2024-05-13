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
#include "shared/rayutils.h"

namespace Editor {
//TODO split up
namespace {
void HandleMoveAction(EditorContext& ec) {
  auto& moveAction = ec.logic.currentMoveAction;
  if (moveAction != nullptr) {
    const auto avgDist = moveAction->calculateDeltas(ec);
    if (avgDist < Logic::MIN_DIST_THRESHOLD) {  // Only create events if enough change
      delete moveAction;
    } else {
      ec.core.addEditorAction(ec, moveAction);
    }
    moveAction = nullptr;
  }
}
}  // namespace

inline void PollControls(EditorContext& ec) {
  const auto mouse = ec.logic.mouse;
  const auto worldMouse = ec.logic.worldMouse;
  auto& dragStart = ec.logic.dragStart;
  auto& camera = ec.display.camera;
  auto& isDragging = ec.logic.isDraggingScreen;
  auto& selectRect = ec.logic.selectRect;
  auto& selectedNodes = ec.core.selectedNodes;
  auto& contextMenuPos = ec.logic.contextMenuPos;

  if (!ec.input.mouseConsumed) {
    camera.zoom += GetMouseWheelMove() * 0.05f;
    camera.zoom = camera.zoom > Display::MAX_ZOOM ? Display::MAX_ZOOM : camera.zoom;
    camera.zoom = camera.zoom < Display::MIN_ZOOM ? Display::MIN_ZOOM : camera.zoom;
  }

  //Context menu
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { contextMenuPos = mouse; }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    if (DistEuclidean(contextMenuPos, mouse) <= 5.0F) {
      ec.logic.showContextMenu = true;
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
    isDragging = false;
    HandleMoveAction(ec);
  }

  if (isDragging) {
    camera.target.x -= worldMouse.x - dragStart.x;
    camera.target.y -= worldMouse.y - dragStart.y;
    dragStart = GetScreenToWorld2D(ec.logic.mouse, camera);
  }

  //Selecting
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !ec.logic.isAnyNodeHovered && !ec.logic.showContextMenu) {
    ec.logic.isSelecting = true;
    ec.logic.selectPoint = worldMouse;
    selectRect.width = 0;
    selectRect.height = 0;
    selectedNodes.clear();
  }

  if (ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT)) { ec.logic.isSelecting = false; }

  //Delete
  if (ec.input.isKeyPressed(KEY_DELETE) || ec.input.isKeyPressed(KEY_BACKSPACE)) { ec.core.erase(ec); }

  //My keyboard...
  //Redo
  if ((ec.input.isKeyPressed(KEY_Z) || ec.input.isKeyPressedRepeat(KEY_Z)) && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    ec.core.redo(ec);
  }

  //Undo
  if ((ec.input.isKeyPressed(KEY_Y) || ec.input.isKeyPressedRepeat(KEY_Y)) && ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    ec.core.undo(ec);
  }

  //CTRL + (V,C,X,+,-,S,O,N) shortcuts
  if (ec.input.isKeyDown(KEY_LEFT_CONTROL)) {
    if (ec.input.isKeyPressed(KEY_C)) {
      ec.core.copy(ec);
    } else if (ec.input.isKeyPressed(KEY_X)) {
      ec.core.cut(ec);
    } else if (ec.input.isKeyPressed(KEY_V)) {
      ec.core.paste(ec);
    } else if (ec.input.isKeyPressed(KEY_S)) {
      bool saveAs = ec.input.isKeyDown(KEY_LEFT_SHIFT);
      ec.persist.saveToFile(ec, saveAs);
    } else if (ec.input.isKeyPressed(KEY_SLASH)) {  // -, _
      ec.display.zoomOut();
    } else if (ec.input.isKeyPressed(KEY_RIGHT_BRACKET)) {  // +, *, ~
      ec.display.zoomIn();
    } else if (ec.input.isKeyPressed(KEY_O)) {
      ec.core.open(ec);
    } else if (ec.input.isKeyPressed(KEY_N)) {
      ec.core.newFile(ec);
    } else if (ec.input.isKeyPressed(KEY_A)) {
      ec.core.selectAll(ec);
    }
  }
}

}  // namespace Editor

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTROLS_H_