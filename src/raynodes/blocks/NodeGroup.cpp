#include "NodeGroup.h"

#include <ranges>
#include <raygui.h>

#include "application/EditorContext.h"
#include "node/Node.h"

static Vector2 DRAG_OFF{};

namespace {
void HandleDrag(EditorContext& ec, NodeGroup& ng) {
  const auto bounds = ng.getBounds();
  const auto mouse = ec.logic.worldMouse;

  if (ng.isDragged && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 delta = ng.pos;
    ng.pos.x = mouse.x - DRAG_OFF.x;
    ng.pos.y = mouse.y - DRAG_OFF.y;
    delta.x -= ng.pos.x;
    delta.y -= ng.pos.y;

    for (auto n : ng.nodes) {
      n->x -= delta.x;
      n->y -= delta.y;
    }

    ec.input.consumeMouse();
  } else {
    ng.isDragged = false;
  }

  if (!ng.isDragged && ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (CheckCollisionPointRec(ec.logic.worldMouse, bounds)) {
      ec.input.consumeMouse();
      ng.isDragged = true;
      DRAG_OFF = {mouse.x - bounds.x, mouse.y - bounds.y};
    }
  }
}
}  // namespace

NodeGroup::NodeGroup(EditorContext& ec, const char* name, std::unordered_map<NodeID, Node*> selectedNodes)
    : name(cxstructs::str_dup(name)) {
  Rectangle bounding{};
  for (const auto n : selectedNodes | std::ranges::views::values) {
    Display::ReduceToBounding(*n, bounding);
    n->isHovered = false;
    n->isDragged = false;
    n->isInGroup = true;
    nodes.push_back(n);
  }
  Display::ApplyInset(bounding, 35);

  unfoldedDims.x = MeasureTextEx(ec.display.editorFont, name, ec.display.fontSize, 0.5F).x + 27;
  unfoldedDims.y = 100;
  pos = {bounding.x, bounding.y};
  dims = {bounding.width, bounding.height};
}

NodeGroup::~NodeGroup() {
  delete[] name;
}

void NodeGroup::draw(EditorContext& ec) {
  const auto mouse = ec.logic.worldMouse;
  const Rectangle bounds = getBounds();
  if (expanded) {
    // Outline
    DrawRectangleLinesEx(bounds, 5, UI::COLORS[N_BACK_GROUND]);
    // Draw nodes
    for (const auto node : nodes) {
      node->isInGroup = false;
      Node::Draw(ec, *node);
      node->isInGroup = true;
    }
    // Draw highlight
    DrawRectangleRec(bounds, ColorAlpha(RAYWHITE, 0.25F));

  } else {
    DrawRectangleRec(bounds, UI::COLORS[N_BACK_GROUND]);
    // Draw nodes
    for (const auto node : nodes) {
      node->isInGroup = false;
      Node::Draw(ec, *node);
      node->isInGroup = true;
    }
  }

  // Draw toggle button
  const auto buttonBounds = Rectangle{pos.x, pos.y, 20, 20};
  const bool hovered = CheckCollisionPointRec(mouse, buttonBounds);
  DrawRectangleRec(buttonBounds, hovered ? UI::Darken(UI::COLORS[UI_MEDIUM]) : UI::COLORS[N_BACK_GROUND]);
  GuiDrawIcon(expanded ? 117 : 116, pos.x + 2, pos.y + 2, 1, UI::COLORS[UI_LIGHT]);
  if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { expanded = !expanded; }

  const auto textPos = Vector2{pos.x + 24, pos.y};
  DrawTextEx(ec.display.editorFont, name, textPos, ec.display.fontSize, 0.5F, UI::COLORS[UI_LIGHT]);
}

void NodeGroup::update(EditorContext& ec) {
  const auto pressed = ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);

  // Only update the pins and call update functions
  for (const auto node : nodes) {
    const auto inX = node->x;
    const auto outX = node->x + node->width;
    auto& n = *node;

    for (auto* comp : n.components) {
      comp->update(ec, n);
      if (!(pressed && expanded)) continue;
      for (auto& in : comp->inputs) {
        Pin::UpdatePin(ec, n, comp, in, inX);
      }
      for (auto& out : comp->outputs) {
        Pin::UpdatePin(ec, n, comp, out, outX);
      }
    }

    // Node update after
    node->update(ec);

    if (!(pressed && expanded)) continue;

    Pin::UpdatePin(ec, n, nullptr, node->nodeIn, inX);
    for (auto& out : node->outputs) {
      Pin::UpdatePin(ec, n, nullptr, out, outX);
    }
  }

  HandleDrag(ec, *this);
}

Rectangle NodeGroup::getBounds() const {
  if (expanded) return {pos.x, pos.y, dims.x, dims.y};
  return {pos.x, pos.y, static_cast<float>(unfoldedDims.x), static_cast<float>(unfoldedDims.y)};
}