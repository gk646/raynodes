#include "Node.h"

#include <ostream>
#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

Node::~Node() {
  for (const auto c : components) {
    c->onDestruction(*this);
    delete c;
  }
}

namespace {
void UpdateComponent(Component* c, EditorContext& ec, Node& n) {
  const auto compRect = c->getBounds();

  //Mouse Enter
  const bool previousHovered = c->isHovered;
  const auto compHovered = CheckCollisionPointRec(ec.logic.worldMouse, compRect);
  c->isHovered = compHovered;

  if (previousHovered != compHovered) {
    if (compHovered) c->onMouseEnter(ec);
    else c->onMouseLeave(ec);
  }

  //Focus Gain
  const bool previousFocused = c->isFocused;
  if (!previousFocused) {
    c->isFocused = compHovered && ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);
  } else {
    cxstructs::Constraint<false> con;  //Constraint holds when all are false

    con += ec.logic.isAnyNodeDragged;
    con += IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !compHovered;
    con += IsMouseButtonPressed(KEY_ESCAPE);

    c->isFocused = con.holds();
  }

  if (previousFocused != c->isFocused) {
    if (c->isFocused) c->onFocusGain(ec);
    else c->onFocusLoss(ec);
  }

  c->update(ec, n);

  //Consume input after update
  if (c->isFocused) {
    ec.input.consumeKeyboard();
    ec.input.consumeMouse();
  }
}

//selectedNodes is std::unordered_map
void HandleSelection(Node& n, EditorContext& ec, const Rectangle bounds, auto& selectedNodes) {
  if (CheckCollisionRecs(bounds, ec.logic.selectRect)) {
    selectedNodes.insert({n.id, &n});
    n.isHovered = true;
    ec.logic.isAnyNodeHovered = true;
    ec.logic.hoveredNode = &n;
  } else {
    n.isHovered = false;
  }
}

bool HandlePinSelection(EditorContext& ec, Node& n) {
  //Left mouse button press has already been checked ->true
  const auto pinY = n.position.y + Node::PADDING + Node::OFFSET_Y + n.contentHeight + 10;
  auto rect = Rectangle{n.position.x + Node::PADDING * 2, pinY, Pin::PIN_SIZE, Pin::PIN_SIZE};
  for (auto& p : n.pins) {
    if (CheckCollisionPointRec(ec.logic.worldMouse, rect)) {
      ec.logic.assignDraggedPin(rect.x + Pin::PIN_SIZE / 2, rect.y + Pin::PIN_SIZE, p, n);
      return true;
    }
    rect.x += Pin::PIN_SIZE + Node::PADDING * 2.0F;
  }
  return false;
}

//selectedNodes is std::unordered_map
void HandleHover(EditorContext& ec, Node& n, auto& selectedNodes) {
  ec.logic.isAnyNodeHovered = true;
  ec.logic.hoveredNode = &n;
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (!ec.input.isKeyDown(KEY_LEFT_CONTROL) && !selectedNodes.contains(n.id)) {
      //Clear selection when an unselected node is clicked / unless control is held
      selectedNodes.clear();
    }

    selectedNodes.insert({n.id, &n});  //Node is clicked = selected

    //If pin is clicked still allow selection but not dragging
    if (HandlePinSelection(ec, n)) return;

    auto& moveAction = ec.logic.currentMoveAction;  //Start a move action
    if (moveAction == nullptr) {                    //Let's not leak too much...
      moveAction = new NodeMovedAction(static_cast<int>(selectedNodes.size()) + 1);
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
void HandleDrag(Node& n, EditorContext& ec, auto& selectedNodes, auto worldMouse) {
  ec.logic.isAnyNodeHovered = true;
  ec.logic.draggedNode = &n;
  if (ec.input.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    const Vector2 movementDelta = {worldMouse.x - Node::DRAG_OFFSET.x, worldMouse.y - Node::DRAG_OFFSET.y};
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

void Node::draw(EditorContext& ec) {
  //Cache
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto worldMouse = ec.logic.worldMouse;

  const auto bounds = Rectangle{position.x, position.y, size.x, size.y};

  DrawRectangleRec(bounds, color);

  if (isHovered) {
    DrawRectangleLinesEx(bounds, 1, ColorAlpha(YELLOW, 0.7));
  }

  const auto headerPos = Vector2{position.x + PADDING, position.y + PADDING};
  DrawTextEx(font, NodeToString(type), headerPos, fs, 1.0F, WHITE);

  const auto startX = position.x + PADDING;
  auto startY = position.y + PADDING + OFFSET_Y;
  const auto beforeY = startY;

  for (const auto c : components) {
    if (!c->internalLabel) {
      DrawTextEx(font, c->name, {startX, startY}, fs, 0.0F, WHITE);
      startY += fs;
    }
    c->x = startX;
    c->y = startY;
    c->draw(ec, *this);
    startY += c->getHeight() + PADDING;
  }

  contentHeight = static_cast<uint16_t>(startY - beforeY);

  startY += 10;
  auto rect = Rectangle{startX + PADDING, startY, Pin::PIN_SIZE, Pin::PIN_SIZE};
  for (int i = 0; i < pins.size(); ++i) {
    DrawRectangleRec(rect, LIGHTGRAY);
    if (CheckCollisionPointRec(worldMouse, rect)) {
      DrawRectangleLinesEx(rect, 1, YELLOW);
    }
    rect.x += Pin::PIN_SIZE + PADDING * 2;
  }

  size.y = startY - position.y + Pin::PIN_SIZE + PADDING;
}
void Node::update(EditorContext& ec) {
  //Cache
  const auto bounds = Rectangle{position.x, position.y, size.x, size.y};
  const auto worldMouse = ec.logic.worldMouse;
  auto& selectedNodes = ec.core.selectedNodes;

  //Always update components to allow for continuous ones (not just when focused)
  float biggestWidth = FLT_MIN;
  for (const auto c : components) {
    UpdateComponent(c, ec, *this);
    if (c->getWidth() > biggestWidth) {
      biggestWidth = static_cast<float>(c->width);
    }
  }
  size.x = biggestWidth + 10.0F;

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
    HandleHover(ec, *this, selectedNodes);
  } else {
    isHovered = !selectedNodes.empty() && selectedNodes.contains(id);
  }

  //Node is dragged
  if (isDragged) {
    HandleDrag(*this, ec, selectedNodes, worldMouse);
  }
}
void Node::saveState(FILE* file) {
  cxstructs::io_save(file, static_cast<int>(type));
  cxstructs::io_save(file, color.r);
  cxstructs::io_save(file, color.g);
  cxstructs::io_save(file, color.b);
  cxstructs::io_save(file, color.a);
  cxstructs::io_save(file, static_cast<int>(position.x));
  cxstructs::io_save(file, static_cast<int>(position.y));
  cxstructs::io_save(file, static_cast<int>(size.x));
  cxstructs::io_save(file, static_cast<int>(size.y));

  for (const auto c : components) {
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
  color = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b),
           static_cast<unsigned char>(a)};

  cxstructs::io_load(file, position.x);
  cxstructs::io_load(file, position.y);
  cxstructs::io_load(file, size.x);
  cxstructs::io_load(file, size.y);

  for (const auto c : components) {
    c->load(file);
  }
}

//Components
void Node::addComponent(Component* comp) {
  components.push_back(comp);
}
Component* Node::getComponent(const char* name) {
  for (const auto c : components) {
    if (c->getName() == name) return c;
  }
  for (const auto c : components) {
    if (strcmp(name, c->getName()) == 0) return c;
  }
  return nullptr;
}

void Node::addPin(const PinType pt, const Direction dir) {
  pins.push_back({nullptr, pt, dir, static_cast<uint8_t>(pins.size())});
}

//Export
void HeaderNode::exportToMQQS(std::ostream& out) {
  out << "Name:" << getComponent("Description")->getString() << "\n";
  // out << "Description:" << description << "\n";
  // out << "Zone:" << zone << "\n";
  //  out << "Level:" << Level << "\n";
}