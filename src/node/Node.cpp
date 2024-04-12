#include "Node.h"

#include <ostream>
#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

namespace {
inline void UpdateComponent(float& x, float& y, Component* c, EditorContext& ec, Node& n) {
  const auto width = c->getWidth();
  const auto height = c->getHeight();
  const auto compRect = Rectangle{x, y, width, height};

  //Mouse Enter
  bool previousHovered = c->isHovered;
  auto compHovered = CheckCollisionPointRec(ec.logic.worldMouse, compRect);
  c->isHovered = compHovered;

  if (previousHovered != compHovered) {
    if (compHovered) c->onMouseEnter(ec);
    else c->onMouseLeave(ec);
  }

  //Focus Gain
  bool previousFocused = c->isFocused;
  if (!previousFocused) {
    c->isFocused = compHovered && ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);
  } else {
    cxstructs::Constraint<false> con;  //Constraint holds when all are false

    con += ec.logic.isAnyNodeDragged && !n.isDragged;
    con += ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && !compHovered;
    con += ec.input.isMouseButtonPressed(KEY_ESCAPE);

    c->isFocused = con.holds();
  }

  if (previousFocused != c->isFocused) {
    if (c->isFocused) c->onFocusGain(ec);
    else c->onFocusLoss(ec);
  }

  c->update(x, y, ec, n);

  if (c->isFocused) {
    ec.input.consumeKeyboard();
    ec.input.consumeMouse();
  }
}

//selectedNodes is std::unordered_map
inline void HandleSelection(Node& n, EditorContext& ec, const Rectangle bounds,
                            auto& selectedNodes) {
  if (CheckCollisionRecs(bounds, ec.logic.selectRect)) {
    selectedNodes.insert({n.id, &n});
    n.isHovered = true;
    ec.logic.isAnyNodeHovered = true;
  } else {
    n.isHovered = false;
  }
}

//selectedNodes is std::unordered_map
inline void HandleHover(Node& n, EditorContext& ec, auto& selectedNodes) {
  ec.logic.isAnyNodeHovered = true;
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (!ec.input.isKeyDown(KEY_LEFT_CONTROL) && !selectedNodes.contains(n.id)) {
      selectedNodes
          .clear();  //Clear selection when an unselected node is clicked / unless control is held
    }

    selectedNodes.insert({n.id, &n});  //Node is clicked = selected

    auto& moveAction = ec.logic.currentMoveAction;  //Start a move action
    if (moveAction == nullptr) {
      moveAction = new NodeMovedAction((int)selectedNodes.size() + 1);
      for (auto pair : selectedNodes) {
        moveAction->movedNodes.emplace_back(pair.first, pair.second->position);
      }
    }

    n.isDragged = true;
    ec.logic.isAnyNodeDragged = true;
    ec.logic.draggedNode = &n;
    Node::DRAG_OFFSET = ec.logic.worldMouse;
  }
}

//selectedNodes is std::unordered_map
inline void HandleDrag(Node& n, EditorContext& ec, auto& selectedNodes, auto worldMouse) {
  ec.logic.isAnyNodeHovered = true;
  ec.logic.draggedNode = &n;
  if (ec.input.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 movementDelta = {worldMouse.x - Node::DRAG_OFFSET.x,
                             worldMouse.y - Node::DRAG_OFFSET.y};
    n.position.x += movementDelta.x;
    n.position.y += movementDelta.y;

    //Update selected nodes
    if (!selectedNodes.empty()) {
      for (auto pair : selectedNodes) {
        Node* node = pair.second;
        if (node != &n) {  // Avoid moving this node again
          // Apply the same movement to all selected nodes
          node->position.x += movementDelta.x;
          node->position.y += movementDelta.y;
        }
      }
    }
    Node::DRAG_OFFSET = worldMouse;
  } else {
    n.isDragged = false;
    ec.logic.isAnyNodeDragged = false;
  }
}
}  // namespace

Node::~Node() {
  for (auto c : components) {
    c->onDestruction(*this);
    delete c;
  }
}
void Node::draw(EditorContext& ec) {
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  const auto rect = Rectangle{position.x, position.y, size.x, size.y};

  DrawRectanglePro(rect, {0, 0}, 0, color);

  if (isHovered) {
    DrawRectangleLinesEx(rect, 1, ColorAlpha(YELLOW, 0.7));
  }

  DrawTextEx(font, NodeToString(type), {position.x + PADDING, position.y + PADDING}, fs, 1.0F,
             WHITE);

  float startX = position.x + PADDING;
  float startY = position.y + OFFSET_Y + PADDING;
  for (auto c : components) {
    c->draw(startX, startY, ec, *this);
    startY += c->getHeight() + PADDING;
  }

  size.y = (startY - position.y) + 10;
}
void Node::update(EditorContext& ec) {
  //Cache
  const auto bounds = Rectangle{position.x, position.y, size.x, size.y};
  const auto worldMouse = ec.logic.worldMouse;
  auto& selectedNodes = ec.core.selectedNodes;

  //Update components
  float startX = position.x + PADDING;
  float startY = position.y + OFFSET_Y + PADDING;

  for (auto c : components) {
    UpdateComponent(startX, startY, c, ec, *this);
    if (c->getWidth() > size.x) {
      size.x = c->getWidth() + 10;
    }
    startY += c->getHeight() + PADDING;
  }

  //User is selecting -> no dragging
  if (ec.logic.isSelecting) {
    return HandleSelection(*this, ec, bounds, selectedNodes);
  }

  //Another node is dragged no point in updating this one
  if (!isDragged && ec.logic.isAnyNodeDragged) {
    //Show as hovered when selected
    isHovered = !selectedNodes.empty() && selectedNodes.contains(id);
    return;
  }

  //Check if hovered
  isHovered = isDragged || CheckCollisionPointRec(worldMouse, bounds);

  //Its hovered - what's going to happen?
  if (isHovered && !ec.logic.isAnyNodeHovered) {
    HandleHover(*this, ec, selectedNodes);
  } else {
    isHovered = !selectedNodes.empty() && selectedNodes.contains(id);
  }

  //Node is dragged
  if (isDragged) {
    HandleDrag(*this, ec, selectedNodes, worldMouse);
  }
}
void Node::saveState(FILE* file) {
  cxstructs::io_save(file, (int)type);
  cxstructs::io_save(file, (int)color.r);
  cxstructs::io_save(file, (int)color.g);
  cxstructs::io_save(file, (int)color.b);
  cxstructs::io_save(file, (int)color.a);
  cxstructs::io_save(file, (int)position.x);
  cxstructs::io_save(file, (int)position.y);
  cxstructs::io_save(file, (int)size.x);
  cxstructs::io_save(file, (int)size.y);

  for (auto c : components) {
    c->save(file);
  }
}
void Node::loadState(FILE* file) {
  //Node type was already parsed
  int r, g, b, a;
  cxstructs::io_load(file, r);
  cxstructs::io_load(file, g);
  cxstructs::io_load(file, b);
  cxstructs::io_load(file, a);
  color = {static_cast<unsigned char>(r), static_cast<unsigned char>(g),
           static_cast<unsigned char>(b), static_cast<unsigned char>(a)};

  cxstructs::io_load(file, position.x);
  cxstructs::io_load(file, position.y);
  cxstructs::io_load(file, size.x);
  cxstructs::io_load(file, size.y);

  for (auto c : components) {
    c->load(file);
  }
}

//Components
void Node::addComponent(Component* comp) {
  components.push_back(comp);
}
Component* Node::getComponent(const char* name) {
  for (auto c : components) {
    if (c->getName() == name) return c;
  }
  for (auto c : components) {
    if (strcmp(name, c->getName()) == 0) return c;
  }
  return nullptr;
}

//Export
void HeaderNode::exportToMQQS(std::ostream& out) {
  out << "Name:" << getComponent("Description")->getString() << "\n";
  // out << "Description:" << description << "\n";
  // out << "Zone:" << zone << "\n";
  //  out << "Level:" << Level << "\n";
}
