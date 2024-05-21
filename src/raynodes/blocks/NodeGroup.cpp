#include "NodeGroup.h"

#include <ranges>

#include "application/EditorContext.h"
#include "node/Node.h"

static Vector2 DRAG_OFF{};

namespace {
void HandleDrag(EditorContext& ec, NodeGroup& ng) {}
void DrawPins(EditorContext& ec, NodeGroup& ng) {}
}  // namespace

NodeGroup::NodeGroup(EditorContext& ec, const char* name, std::unordered_map<NodeID, Node*> selectedNodes)
    : name(cxstructs::str_dup(name)) {
  Rectangle bounding{};
  for (const auto n : selectedNodes | std::ranges::views::values) {
    Display::ReduceToBounding(*n, bounding);
    n->isHovered = false;
    n->isDragged = false;
    nodes.push_back(n);
    ec.core.removeNode(ec, n->uID);
  }
  Display::ApplyInset(bounding, 50);

  unfoldedWidth = MeasureTextEx(ec.display.editorFont, name, ec.display.fontSize, 0.5F).x;
  pos = {bounding.x, bounding.y};
  dims = {bounding.width, bounding.height};
}

NodeGroup::~NodeGroup() {
  delete[] name;
  for (const auto n : nodes) {
    delete n;
  }
}

void NodeGroup::draw(EditorContext& ec) {
  if (expanded) {
    const Rectangle bounds = {pos.x, pos.y, dims.x, dims.y};
    DrawRectangleLinesEx(bounds, 5, UI::COLORS[UI_MEDIUM]);
    // Draw nodes
    for (const auto node : nodes) {
      Node::Draw(ec, *node);
    }
    DrawRectangleRec(bounds, ColorAlpha(RAYWHITE, 0.3F));
    const auto textPos = Vector2{pos.x + 24, pos.y};
    DrawTextEx(ec.display.editorFont, name, textPos, ec.display.fontSize, 0.5F, UI::COLORS[UI_LIGHT]);
  } else {
    const Rectangle bounds = {pos.x, pos.y, static_cast<float>(unfoldedWidth), 100};
    DrawRectangleRec(bounds, ColorAlpha(RAYWHITE, 0.3F));
  }
}

void NodeGroup::update(EditorContext& ec) {
  HandleDrag(ec, *this);
  const auto pressed = ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);

  // Manually only update the pins and call update functions
  for (const auto node : nodes) {
    const auto inX = node->x;
    const auto outX = node->x + node->width;
    auto& n = *node;

    node->update(ec);

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
    if (!(pressed && expanded)) continue;

    Pin::UpdatePin(ec, n, nullptr, node->nodeIn, inX);
    for (auto& out : node->outputs) {
      Pin::UpdatePin(ec, n, nullptr, out, outX);
    }
  }
}