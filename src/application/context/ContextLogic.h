#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_

struct NodeMovedAction;

struct Logic {
  static constexpr int MIN_DIST_THRESHOLD = 5;
  Rectangle selectRect = {0};                    //Current selection rectangle
  NodeMovedAction* currentMoveAction = nullptr;  //All move actions are sequential
  Node* draggedNode = nullptr;                   //Currently dragged node
  Node* hoveredNode = nullptr;                   //Currently hovered node
  Node* draggedPinStartNode = nullptr;           //Only assigned if "isMakingConnection" holds
  Vector2 dragStart = {0.0f, 0.0f};
  Vector2 selectPoint = {0.0f, 0.0f};
  Vector2 mouse = {0};       //Mouse pos in screen space
  Vector2 worldMouse = {0};  //Mouse pos in world space
  Vector2 contextMenuPos = {};
  Vector2 draggedPinPos = {};  //Position of the dragged pin
  Pin* draggedPin = nullptr;   //Start pin - Always valid if "isMakingConnection" holds

  bool isMakingConnection = false;  //Is user trying to connect pins
  bool isDraggingScreen = false;
  bool showContextMenu = false;
  bool isSelecting = false;  //Is user currently selecting
  bool isAnyNodeHovered = false;
  bool isAnyNodeDragged = false;

  inline void assignDraggedPin(float x, float y, Pin& p, Node& n) {
    draggedPinStartNode = &n;
    draggedPin = &p;
    draggedPinPos = {x, y};
    isMakingConnection = true;
  }
  void handleDroppedPin(EditorContext& ec);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
