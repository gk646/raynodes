#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTLOGIC_H_

struct NodeMovedAction;

struct Logic {
  static constexpr int MIN_DIST_THRESHOLD = 5;
  Rectangle selectRect = {};                     //Current selection rectangle
  NodeMovedAction* currentMoveAction = nullptr;  //All move actions are sequential
  Node* draggedNode = nullptr;                   //Currently dragged node
  Node* hoveredNode = nullptr;                   //Currently hovered node
  Node* draggedPinNode = nullptr;                //Only assigned if "isMakingConnection" holds
  Component* draggedPinComponent = nullptr;      //Only assigned if "isMakingConnection" holds
  Vector2 dragStart = {0.0f, 0.0f};
  Vector2 selectPoint = {0.0f, 0.0f};
  Vector2 mouse = {};       //Mouse pos in screen space
  Vector2 worldMouse = {};  //Mouse pos in world space
  Vector2 contextMenuPos = {};
  Vector2 draggedPinPos = {};  //Position of the dragged pin
  Pin* draggedPin = nullptr;   //Start pin - Always valid if "isMakingConnection" holds

  bool isMakingConnection = false;  //Is user trying to connect pins
  bool isDraggingScreen = false;
  bool showContextMenu = false;
  bool isSelecting = false;  //Is user currently selecting
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