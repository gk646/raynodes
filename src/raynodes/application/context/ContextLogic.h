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

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_

struct Logic {
  static constexpr int MIN_DIST_THRESHOLD = 5;   // Minmal move distance for a node move action to trigger
  Rectangle selectRect = {};                     //Current selection rectangle
  NodeMovedAction* currentMoveAction = nullptr;  //All move actions are sequential
  Node* draggedNode = nullptr;                   //Currently dragged node
  Node* hoveredNode = nullptr;                   //Currently hovered node
  Node* draggedPinNode = nullptr;                //Only assigned if "isMakingConnection" holds
  Component* draggedPinComponent = nullptr;      //Only assigned if "isMakingConnection" holds
  Pin* draggedPin = nullptr;                     //Start pin - Always valid if "isMakingConnection" holds
  Vector2 dragStart = {0.0f, 0.0f};
  Vector2 selectPoint = {0.0f, 0.0f};
  Vector2 mouse = {};           // Mouse pos in screen space
  Vector2 worldMouse = {};      // Mouse pos in world space
  Vector2 contextMenuPos = {};  // Position of the context menu
  Vector2 draggedPinPos = {};   // Position of the dragged pin
  bool isMakingConnection = false;  // Is user trying to connect pins
  bool isDraggingScreen = false;
  bool showContextMenu = false;
  bool isSelecting = false;  // Is user currently selecting
  bool isAnyNodeHovered = false;
  bool isAnyNodeDragged = false;

  void assignDraggedPin(const float x, const float y, Node& n, Component& c, Pin& p) {
    draggedPinNode = &n;
    draggedPinComponent = &c;
    draggedPin = &p;
    draggedPinPos = {x, y};
    isMakingConnection = true;
  }
  void handleDroppedPin(EditorContext& ec);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_