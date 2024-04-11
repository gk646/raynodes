#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_

struct NodeMovedAction;
struct Logic {
  Rectangle selectRect = {0};                    //Current selection rectangle
  NodeMovedAction* currentMoveAction = nullptr;  //All move actions are sequential
  Node* draggedNode = nullptr;                   //Currently dragged node
  Vector2 dragStart = {0.0f, 0.0f};
  Vector2 selectPoint = {0.0f, 0.0f};
  Vector2 mouse = {0};       //Mouse pos in screen space
  Vector2 worldMouse = {0};  //Mouse pos in world space
  Vector2 contextMenuPos = {};
  bool isDraggingScreen = false;
  bool showContextMenu = false;
  bool isSelecting = false;  //Is user currently selecting
  bool isAnyNodeHovered = false;
  bool isDraggingNode = false;
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
