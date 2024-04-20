#include "Node.h"

#include <ostream>
#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"
#include "shared/rayutils.h"

#include <shared/fwd.h>

Node::~Node() {
  for (const auto c : components) {
    c->onDestruction(*this);
    delete c;
  }
}

namespace {
bool CheckPinCollisions(EditorContext& ec, Node& n, Component* c) {
  const auto worldMouse = ec.logic.worldMouse;
  float radius = Pin::PIN_SIZE / 2.0F;
  float posX = n.position.x;

  //Inputs on the left
  for (auto& p : c->inputs) {
    if (CheckCollisionPointCircle(worldMouse, {posX, p.yPos}, radius)) {
      ec.logic.assignDraggedPin(posX, p.yPos, n, *c, p);
      return true;
    }
  }

  //Outputs on the right
  posX += n.size.x;
  for (auto& p : c->outputs) {
    if (CheckCollisionPointCircle(worldMouse, {posX, p.yPos}, radius)) {
      ec.logic.assignDraggedPin(posX, p.yPos, n, *c, p);
      return true;
    }
  }

  return false;
}

void UpdateComponent(EditorContext& ec, Node& n, Component* c) {
  const auto compRect = c->getBounds();
  const auto worldMouse = ec.logic.worldMouse;

  //Pins are partly outside so need to check them regardless of hover state
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //First check if the mouse is close to the rect and mouse is pressed
    //TODO this is done for all components / only needs to be done once
    if (CheckCollisionPointRec(worldMouse, n.getExtendedBounds(Pin::PIN_SIZE))) {
      if (CheckPinCollisions(ec, n, c)) {
        //Consume input already
        ec.input.consumeKeyboard();
        ec.input.consumeMouse();
      }
    }
  }

  //Mouse Enter
  const bool previousHovered = c->isHovered;
  const auto compHovered = CheckCollisionPointRec(worldMouse, compRect);
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

void DrawComponentPins(EditorContext& ec, Component& c, float dx, float startIn, float startOut, float w) {
  constexpr float pinRadius = Pin::PIN_SIZE / 2.0f;

  // Draw Input Pins
  float currentY = startIn;
  bool isAltDown = ec.input.isKeyDown(KEY_LEFT_ALT);
  for (auto& p : c.inputs) {
    const auto middlePos = Vector2{dx, currentY + pinRadius};
    if (isAltDown) {
      auto txt = PinTypeToString(p.pinType);
      const auto textPos = Vector2{middlePos.x - Pin::PIN_SIZE, middlePos.y - Pin::PIN_SIZE};
      DrawCenteredText(ec.display.editorFont, txt, textPos, Pin::PIN_SIZE, 1.0F, WHITE);
    }
    DrawCircleV({dx, currentY + pinRadius}, pinRadius, p.getColor());
    p.yPos = currentY + pinRadius;
    currentY += Pin::PIN_SIZE;
  }

  // Draw Output Pins
  currentY = startOut;
  float outputX = dx + w;  // Pins on the right edge
  for (auto& p : c.outputs) {
    const auto middlePos = Vector2{outputX, currentY + pinRadius};
    if (isAltDown) {
      auto txt = PinTypeToString(p.pinType);
      const auto textPos = Vector2{middlePos.x + Pin::PIN_SIZE, middlePos.y - Pin::PIN_SIZE};
      DrawCenteredText(ec.display.editorFont, txt, textPos, Pin::PIN_SIZE, 1.0F, WHITE);
    }
    DrawCircleV({outputX, currentY + pinRadius}, pinRadius, p.getColor());
    p.yPos = currentY + pinRadius;
    currentY += Pin::PIN_SIZE;
  }
}

void DrawComponent(EditorContext& ec, Node& n, Component& c, float dx, float& dy, float width) {
  constexpr float pinRadius = Pin::PIN_SIZE / 2.0f;
  int maxPins = std::max(c.inputs.size(), c.outputs.size());
  float componentHeight = c.getHeight();

  // Calculate the total height needed for pins on the side with the maximum number
  float totalPinHeight = maxPins * Pin::PIN_SIZE;
  float maxVerticalSpace = std::max(totalPinHeight, componentHeight);

  // Calculate starting positions for pins and component to be vertically centered
  float startYInputs = dy + (maxVerticalSpace - totalPinHeight) / 2.0f;
  float startYOutputs = startYInputs;  // Symmetrical layout
  float componentStartY = dy + (maxVerticalSpace - componentHeight) / 2.0f;

  DrawComponentPins(ec, c, dx, startYInputs, startYOutputs, width);

  c.x = dx + Node::PADDING * 2;
  c.y = componentStartY;
  // Draw the component itself, centered
  c.draw(ec, n);

  // Update dy for the next component
  dy += maxVerticalSpace + Node::PADDING;
}
}  // namespace

void Node::draw(EditorContext& ec) {
  const auto& display = ec.display;
  const auto bounds = Rectangle{position.x, position.y, size.x, size.y};
  const auto headerPos = Vector2{position.x + PADDING, position.y + PADDING};
  float startY = position.y + PADDING + OFFSET_Y;

  // Draw node body
  DrawRectangleRec(bounds, color);

  // Draw hover outline
  if (isHovered) {
    DrawRectangleLinesEx(bounds, 1, ColorAlpha(YELLOW, 0.7));
  }

  // Draw header text
  DrawTextEx(display.editorFont, NodeToString(type), headerPos, display.fontSize, 1.0F, WHITE);

  // Iterate over components and draw them
  const float initialY = startY;
  for (const auto& component : components) {
    if (!component->internalLabel) {
      Vector2 textPos = {position.x + PADDING, startY};
      DrawTextEx(display.editorFont, component->name, textPos, display.fontSize, 0.0F, WHITE);
      startY += display.fontSize;
    }
    DrawComponent(ec, *this, *component, position.x, startY, size.x);
  }

  contentHeight = static_cast<uint16_t>(startY - initialY);
  size.y = startY - position.y + Pin::PIN_SIZE + PADDING;
}

void Node::update(EditorContext& ec) {
  //Cache
  const auto bounds = Rectangle{position.x, position.y, size.x, size.y};
  const auto worldMouse = ec.logic.worldMouse;
  auto& selectedNodes = ec.core.selectedNodes;

  //Always update components to allow for continuous ones (not just when focused)
  float biggestWidth = FLT_MIN;
  for (auto* c : components) {
    UpdateComponent(ec, *this, c);
    if (c->getWidth() > biggestWidth) {
      biggestWidth = static_cast<float>(c->width);
    }
  }
  size.x = biggestWidth + Node::PADDING * 4.0F;  // Components are drawn with 2*PADDING inset

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
  cxstructs::io_save(file, id);
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
  //Node id was already parsed
  this->id = static_cast<NodeID>(id);
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

//Export
void HeaderNode::exportToMQQS(std::ostream& out) {
  out << "Name:" << getComponent("Description")->getString() << "\n";
  // out << "Description:" << description << "\n";
  // out << "Zone:" << zone << "\n";
  //  out << "Level:" << Level << "\n";
}