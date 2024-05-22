#include "NodeGroup.h"

#include <ranges>
#include <raygui.h>

#include "application/EditorContext.h"
#include "node/Node.h"

static Vector2 DRAG_OFF{};

// Planned Feature List
// - Nodes can be dragged into and out of a group
// - Nodes can be freely moved inside the group and connected
// - Inputs and outputs are generated automatically
//   -> Nodes that have no inputs will have their inputs projected to the group start
//   -> Nodes that have no outputs will have their outputs projected to the group end
// - Groups can be copied
//   - need to implement to correctly deep copy relative connection between copied nodes
// - This all allows them to be correctly exported and queried like normal nodes but optimize space and complexity

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
bool HasNoInputs(const Component& c) {
  for (const auto& in : c.inputs) {
    if (in.connection != nullptr) return false;
  }
  return true;
}
bool HasNoOutputs(const Node* search, const std::vector<Node*>& others) {
  for (const auto n : others) {
    for (const auto& c : n->components) {
      for (const auto& in : c->inputs) {
        if (in.connection != nullptr && &in.connection->fromNode == search) return false;
      }
    }
  }
  return true;
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

  foldedDims.x = MeasureTextEx(ec.display.editorFont, name, ec.display.fontSize, 0.5F).x + 27;
  foldedDims.y = 100;
  pos = {bounding.x, bounding.y};
  dims = {bounding.width, bounding.height};

  usedPins.reserve(5);
  for (const auto node : nodes) {
    // Draw inputs
    for (const auto comp : node->components) {
      if (HasNoInputs(*comp)) {
        for (auto& in : comp->inputs) {
          usedPins.emplace_back(node, comp, &in);
        }
      }
    }
    // Draw outputs
    if (HasNoOutputs(node, nodes)) {
      for (const auto comp : node->components) {
        for (auto& out : comp->outputs) {
          usedPins.emplace_back(node, comp, &out);
        }
      }
    }
  }
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
    const auto inX = pos.x;
    const auto outX = pos.x + foldedDims.x;
    // Draw pins
    float inY = pos.y + 25;
    float outY = pos.y + 25;
    const auto& f = ec.display.editorFont;
    const auto showText = ec.input.isKeyDown(KEY_LEFT_ALT);

    for (auto [node, comp, pin] : usedPins) {
      if (pin->direction == INPUT) {
        Pin::DrawPin(*pin, f, inX, inY, showText);
        inY += Pin::PIN_SIZE;
      } else {
        Pin::DrawPin(*pin, f, outX, outY, showText);
        outY += Pin::PIN_SIZE;
      }
    }
  }

  // Draw toggle button
  const auto buttonBounds = Rectangle{pos.x, pos.y, 20, 20};
  const bool hovered = CheckCollisionPointRec(mouse, buttonBounds);
  DrawRectangleRec(buttonBounds, hovered ? UI::Darken(UI::COLORS[UI_MEDIUM]) : UI::COLORS[N_BACK_GROUND]);
  GuiDrawIcon(expanded ? 117 : 116, pos.x + 2, pos.y + 2, 1, UI::COLORS[UI_LIGHT]);
  if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    expanded = !expanded;
    for (const auto node : nodes) {
      for (const auto comps : node->components) {
        for (auto& p : comps->outputs) {
          p.xPos = FLT_MIN;
        }
      }
    }
  }

  const auto textPos = Vector2{pos.x + 24, pos.y};
  DrawTextEx(ec.display.editorFont, name, textPos, ec.display.fontSize, 0.5F, UI::COLORS[UI_LIGHT]);
}

void NodeGroup::update(EditorContext& ec) {
  const auto pressed = ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT);

  if (expanded) {
    // Update the node normally
    for (const auto node : nodes) {
      node->isInGroup = false;
      Node::Update(ec, *node);
      node->isInGroup = true;
    }
  } else {
    // Only update the pins and call update functions
    for (const auto node : nodes) {
      auto& n = *node;
      for (auto* comp : n.components) {
        const float x = comp->x;
        comp->x = FLT_MAX;
        comp->update(ec, n);
        comp->x = x;
      }
      // Node update after
      const float x = node->x;
      node->x = FLT_MAX;
      node->update(ec);
      node->x = x;
    }

    if (pressed) {
      const auto inX = pos.x;
      const auto outX = pos.x + foldedDims.x;
      for (auto [node, comp, pin] : usedPins) {
        if (pin->direction == INPUT) {
          Pin::UpdatePin(ec, *node, comp, *pin, inX);
        } else {
          Pin::UpdatePin(ec, *node, comp, *pin, outX);
        }
      }
    }
  }
  HandleDrag(ec, *this);
}

Rectangle NodeGroup::getBounds() const {
  if (expanded) return {pos.x, pos.y, dims.x, dims.y};
  return {pos.x, pos.y, static_cast<float>(foldedDims.x), static_cast<float>(foldedDims.y)};
}