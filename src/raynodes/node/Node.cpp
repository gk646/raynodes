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

#include "Node.h"

#include <cfloat>
#include <raylib.h>
#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"
#include "shared/fwd.h"

// Those are only used in this translation unit
static constexpr float PADDING = 3;
static constexpr float OFFSET_Y = 20;  // Node name header offset
static Vector2 DRAG_OFFSET;            // Drag anchor point
static constexpr float MIN_WIDTH = 75;
static constexpr float MIN_HEIGHT = 45;
static float CACHED_ZOOM = 1.0F;  // Cache the zoom value to have cleaner method parameters

#define LOW_ZOOM_THRESHOLD 0.4F
// We make this likely - zoom is low we might miss the branch but gain the skip
#define IF_HIGH_ZOOM(expr)                                                                                          \
  if (CACHED_ZOOM > LOW_ZOOM_THRESHOLD) [[likely]] { expr; }

// Helper functions
namespace {
void UpdateNodePins(EditorContext& ec, Node& n) {
  if (!ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

  if (Pin::UpdatePin(ec, n, nullptr, n.nodeIn, n.x)) return;

  for (auto& p : n.outputs) {
    if (Pin::UpdatePin(ec, n, nullptr, p, n.x + n.width)) return;
  }
}

void DrawNodePins(const EditorContext& ec, Node& n) {
  float posY = n.y + OFFSET_Y / 2.0F;
  const auto& font = ec.display.editorFont;
  const bool showText = IsKeyDown(KEY_LEFT_ALT);

  Pin::DrawPin(n.nodeIn, font, n.x, posY, showText);
  for (auto& p : n.outputs) {
    Pin::DrawPin(p, font, n.x + n.width, posY, showText);
    posY += Pin::PIN_SIZE;
  }
}

void CheckPinCollisions(EditorContext& ec, Node& n, Component* c) {
  // Inputs on the left
  for (auto& p : c->inputs) {
    if (Pin::UpdatePin(ec, n, c, p, n.x)) return;
  }

  // Outputs on the right
  for (auto& p : c->outputs) {
    if (Pin::UpdatePin(ec, n, c, p, n.x + n.width)) return;
  }
}

void UpdateComponent(EditorContext& ec, Node& n, Component* c) {
  const auto compRect = c->getBounds();
  const auto worldMouse = ec.logic.worldMouse;
  const auto pressed = ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);

  //Pins are partly outside so need to check them regardless of hover state
  if (pressed) {
    //First check if the mouse is close to the rect and mouse is pressed
    //TODO this check is done for all components / only needs to be done once
    if (CheckCollisionPointRec(worldMouse, n.getExtendedBounds(Pin::PIN_SIZE))) { CheckPinCollisions(ec, n, c); }
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
    c->isFocused = compHovered && pressed;
  } else if (ec.logic.isAnyNodeHovered || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
             || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !compHovered) || IsKeyPressed(KEY_ESCAPE)) {
    c->isFocused = false;
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
    selectedNodes.insert({n.uID, &n});
    n.isHovered = true;
    ec.logic.isAnyNodeHovered = true;
    ec.logic.hoveredNode = &n;
  } else {
    selectedNodes.erase(n.uID);
    n.isHovered = false;
  }
}

//selectedNodes is std::unordered_map
void HandleHover(EditorContext& ec, Node& n, std::unordered_map<NodeID, Node*>& selectedNodes) {
  ec.logic.isAnyNodeHovered = true;
  ec.logic.hoveredNode = &n;
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (!ec.input.isKeyDown(KEY_LEFT_CONTROL) && !selectedNodes.contains(n.uID)) {
      //Clear selection when an unselected node is clicked / unless control is held
      selectedNodes.clear();
    }

    selectedNodes.insert({n.uID, &n});  //Node is clicked = selected

    auto& moveAction = ec.logic.currentMoveAction;  //Start a move action
    if (moveAction == nullptr) {                    //Let's not leak too much...
      moveAction = new NodeMovedAction(static_cast<int>(selectedNodes.size()) + 1);
      for (auto [id, node] : selectedNodes) {
        moveAction->movedNodes.push_back({id, {node->x, node->y}});
      }
    }

    n.isDragged = true;
    ec.logic.isAnyNodeDragged = true;
    ec.logic.draggedNode = &n;
    DRAG_OFFSET = ec.logic.worldMouse;
  }
}

//selectedNodes is std::unordered_map
void HandleDrag(Node& n, EditorContext& ec, auto& selectedNodes, auto worldMouse) {
  ec.logic.isAnyNodeHovered = true;
  ec.logic.isAnyNodeDragged = true;
  ec.logic.draggedNode = &n;
  if (ec.input.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    const Vector2 movementDelta = {worldMouse.x - DRAG_OFFSET.x, worldMouse.y - DRAG_OFFSET.y};
    n.x += movementDelta.x;
    n.y += movementDelta.y;

    //Update selected nodes
    if (!selectedNodes.empty()) {
      for (auto pair : selectedNodes) {
        Node* node = pair.second;
        if (node != &n) {  // Avoid moving this node again
          // Apply the same movement to all selected nodes
          node->x += movementDelta.x;
          node->y += movementDelta.y;
        }
      }
    }
    DRAG_OFFSET = worldMouse;
  } else {
    n.isDragged = false;
    ec.logic.isAnyNodeDragged = false;
  }
}

void DrawComponentPins(const EditorContext& ec, Component& c, const float dx, const float in, const float out,
                       const float w) {
  const bool showText = ec.input.isKeyDown(KEY_LEFT_ALT);
  const auto& font = ec.display.editorFont;
  // Draw Input Pins
  float currentY = in;
  for (auto& p : c.inputs) {
    Pin::DrawPin(p, font, dx, currentY, showText);
    currentY += Pin::PIN_SIZE;
  }

  // Draw Output Pins
  currentY = out;
  const float outputX = dx + w;  // Pins on the right edge
  for (auto& p : c.outputs) {
    Pin::DrawPin(p, font, outputX, currentY, showText);
    currentY += Pin::PIN_SIZE;
  }
}

void DrawComponent(EditorContext& ec, Node& n, Component& c, const float dx, float& dy, const float width) {
  if (!c.internalLabel) [[likely]] {
    const auto fs = ec.display.fontSize;
    const Vector2 textPos = {n.x + PADDING, dy};
    IF_HIGH_ZOOM(DrawTextEx(ec.display.editorFont, c.label, textPos, fs, 0.0F, UI::COLORS[UI_LIGHT]));
    dy += fs;
  }

  const int maxPins = std::max(c.inputs.size(), c.outputs.size());
  const float componentHeight = c.getHeight();

  // Calculate the total height needed for pins on the side with the maximum number
  const float totalPinHeight = static_cast<float>(maxPins) * Pin::PIN_SIZE;
  const float maxVerticalSpace = std::max(totalPinHeight, componentHeight);

  // Calculate starting positions for pins and component to be vertically centered
  const float startYInputs = dy + (maxVerticalSpace - totalPinHeight) / 2.0f;
  const float startYOutputs = startYInputs;  // Symmetrical layout
  const float componentStartY = dy + (maxVerticalSpace - componentHeight) / 2.0f;

  IF_HIGH_ZOOM(DrawComponentPins(ec, c, dx, startYInputs, startYOutputs, width));

  c.x = dx + PADDING * 2;
  c.y = componentStartY;
  // Draw the component itself, centered
  c.draw(ec, n);

  // Update dy for the next component
  dy += maxVerticalSpace + PADDING;
}
}  // namespace

Node::Node(const NodeTemplate& nt, const Vec2 pos, const NodeID id)
    : x(pos.x), y(pos.y), width(50), height(50), color(0, 0, 0, 255), name(nt.label), uID(id) {
  outputs.push_back(OutputPin{NODE});
}
Node::Node(const Node& n, const NodeID id)
    : x(n.x), y(n.y), width(n.width), height(n.height), color(n.color), name(n.name), uID(id) {
  for (const auto c : n.components) {
    auto clone = c->clone();
    for (auto& in : clone->inputs) {
      in.connection = nullptr;  //Dont copy the connection ptr
      //TODO maybe copy connections but then fully -> create new connection objects
    }
    components.push_back(clone);
  }
  outputs.push_back(OutputPin{NODE});
}
Node::~Node() {
  for (const auto c : components) {
    c->onDestruction(*this);
    delete c;
  }
}
Node* Node::clone(const NodeID nid) {
  return new Node(*this, nid);
}

void Node::Draw(EditorContext& ec, Node& n) {
  if (n.isInGroup) [[unlikely]] { return; }
  const auto& f = ec.display.editorFont;
  CACHED_ZOOM = ec.display.camera.zoom;
  const auto fs = ec.display.fontSize;
  const auto bounds = Rectangle{n.x, n.y, n.width, n.height};
  const auto headerPos = Vector2{n.x + PADDING * 3.0F, n.y + PADDING};
  float startY = n.y + PADDING + OFFSET_Y;

  // Draw node body
  DrawRectangleRec(bounds, UI::COLORS[N_BACK_GROUND]);

  // Draw hover outline
  if (n.isHovered) [[unlikely]] { DrawRectangleLinesEx(bounds, 1, ColorAlpha(UI::COLORS[UI_LIGHT], 0.7)); }

  // Draw header text
  IF_HIGH_ZOOM(DrawTextEx(f, n.name, headerPos, fs, 1.0F, UI::COLORS[UI_LIGHT]));

  // Draw Node pins
  IF_HIGH_ZOOM(DrawNodePins(ec, n));

  // Iterate over components and draw them
  const float initialY = startY;
  for (const auto component : n.components) {
    DrawComponent(ec, n, *component, n.x, startY, n.width);
  }

  n.contentHeight = static_cast<uint16_t>(startY - initialY);
  n.height = std::max(startY - n.y + Pin::PIN_SIZE + PADDING, MIN_HEIGHT);

  n.draw(ec);  // Call event func last
}
void Node::Update(EditorContext& ec, Node& n) {
  if (n.isInGroup) [[unlikely]] { return; }

  //Cache
  const auto bounds = Rectangle{n.x, n.y, n.width, n.height};
  const auto worldMouse = ec.logic.worldMouse;
  auto& selectedNodes = ec.core.selectedNodes;

  // Update node-level pins
  UpdateNodePins(ec, n);

  //Always update components to allow for continuous ones (not just when focused)
  float biggestWidth = FLT_MIN;
  for (auto* c : n.components) {
    UpdateComponent(ec, n, c);
    if (c->getWidth() > biggestWidth) { biggestWidth = static_cast<float>(c->width); }
  }

  // Components are drawn with 2 * PADDING inset (both sides)
  n.width = std::max(biggestWidth + PADDING * 4.0F, MIN_WIDTH);

  n.update(ec);  // Call event func after components

  //User is selecting -> no dragging
  if (ec.logic.isSelecting) [[unlikely]] { return HandleSelection(n, ec, bounds, selectedNodes); }

  //Another node is dragged no point in updating this one
  if (!n.isDragged && ec.logic.isAnyNodeDragged) {
    //Show as hovered when selected
    n.isHovered = !selectedNodes.empty() && selectedNodes.contains(n.uID);
    return;
  }

  //Check if hovered
  n.isHovered = n.isDragged || CheckCollisionPointRec(worldMouse, bounds);

  //Its hovered - what's going to happen?
  if (n.isHovered && !ec.logic.isAnyNodeHovered) [[unlikely]] {
    HandleHover(ec, n, selectedNodes);
  } else {
    n.isHovered = !selectedNodes.empty() && selectedNodes.contains(n.uID);
  }

  //Node is dragged
  if (n.isDragged) [[unlikely]] { HandleDrag(n, ec, selectedNodes, worldMouse); }
}
void Node::SaveState(FILE* file, const Node& n) {
  cxstructs::io_save(file, n.uID);

  // Save components first for faster access time when importing
  for (const auto c : n.components) {
    c->save(file);
    fputc('\035', file);  // Save where the component ends - Group Separator ASCII
  }

  n.saveState(file);

  cxstructs::io_save(file, static_cast<int>(n.x));
  cxstructs::io_save(file, static_cast<int>(n.y));
}
void Node::LoadState(FILE* file, Node& n) {
  //Node name (type) was already parsed
  //Node id was already parsed

  //Components first
  for (const auto c : n.components) {
    c->load(file);
    fseek(file, 1, SEEK_CUR);  // Skip the component end marker
  }

  n.loadState(file);

  cxstructs::io_load(file, n.x);
  cxstructs::io_load(file, n.y);
}

// Components
void Node::addComponent(Component* comp) {
  components.push_back(comp);
}

Component* Node::getComponent(const char* label) {
  for (const auto c : components) {
    if (strcmp(label, c->getLabel()) == 0) return c;
  }
  return nullptr;
}

// Helpers
Rectangle Node::getExtendedBounds(const float ext) const {
  return {x - ext, y - ext, width + ext * 2, height + ext * 2};
}
Rectangle Node::getBounds() const {
  return {x, y, width, height};
}
Color Node::getColor() const {
  return {color.r, color.g, color.b, color.a};
}