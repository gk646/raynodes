#include "NodeGroup.h"

#include <ranges>
#include <algorithm>
#include <raygui.h>

#include "application/EditorContext.h"
#include "node/Node.h"
#include "ui/elements/PopupMenu.h"

static Vector2 DRAG_OFF{};

// Feature List
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

    for (const auto n : ng.nodes) {
      n->x -= delta.x;
      n->y -= delta.y;
    }

    ec.input.consumeMouse();
  } else {
    ng.isDragged = false;
  }

  if (CheckCollisionPointRec(ec.logic.worldMouse, bounds)) {
    ng.isHovered = true;
    ec.logic.hoveredGroup = &ng;
    if (!ng.isDragged && ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) {
      ec.input.consumeMouse();
      ng.isDragged = true;
      DRAG_OFF = {mouse.x - bounds.x, mouse.y - bounds.y};
    }
  } else {
    ng.isHovered = false;
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
void DrawRename(EditorContext& ec, NodeGroup& ng) {
  const auto validFunc = [](EditorContext& ec, const char* usrStr) {
    const int len = strlen(usrStr);
    if (len == 0) return "Name is too short!";
    if (len >= PLG_MAX_NAME_LEN) return "Name is too short!";
    return static_cast<const char*>(nullptr);
  };

  constexpr auto bounds = UI::GetCenteredWindowBounds<false>();
  constexpr auto header = "Name the node group!";
  const auto res = PopupMenu::InputText(ec, bounds, ec.ui.miscField, validFunc, header);
  if (res == nullptr) return;
  if (res == UI::DUMMY_STRING) ng.isRenaming = false;
  else {
    delete[] ng.name;
    ng.name = cxstructs::str_dup(res);
  }
}
}  // namespace

NodeGroup::NodeGroup(EditorContext& ec, const char* name, std::unordered_map<NodeID, Node*> selectedNodes)
    : foldedDims(), pos(), dims(), name(cxstructs::str_dup(name)) {
  usedPins.reserve(5);
  nodes.reserve(selectedNodes.size() + 1);

  for (const auto n : selectedNodes | std::ranges::views::values) {
    if (!n->isInGroup) nodes.push_back(n);
  }

  updateInternalState(ec);
}

NodeGroup::NodeGroup(EditorContext& ec, const char* name)
    : foldedDims(), pos(), dims(), name(cxstructs::str_dup(name)) {
  updateInternalState(ec);
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
    const auto midX = pos.x + foldedDims.x / 2.0F;
    const auto midY = pos.y + foldedDims.y / 2.0F;
    float inY = pos.y + 25;
    float outY = pos.y + 25;

    const auto& f = ec.display.editorFont;
    const auto showText = ec.input.isKeyDown(KEY_LEFT_ALT);

    // Center all pins
    for (const auto node : nodes) {
      // Center node pins
      node->nodeIn.xPos = midX;
      node->nodeIn.yPos = midY;
      for (auto& p : node->outputs) {
        p.xPos = midX;
        p.yPos = midY;
      }
      for (auto* comp : node->components) {
        // Center free pins
        for (auto& p : comp->outputs) {
          p.xPos = midX;
          p.yPos = midY;
        }
        for (auto& p : comp->inputs) {
          p.xPos = midX;
          p.yPos = midY;
        }
      }
    }

    // Properly align usedPins
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
  if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { expanded = !expanded; }

  const auto textPos = Vector2{pos.x + 24, pos.y};
  DrawTextEx(ec.display.editorFont, name, textPos, ec.display.fontSize, 0.5F, UI::COLORS[UI_LIGHT]);

  if (isHovered) [[unlikely]] { DrawRectangleLinesEx(bounds, 1, UI::COLORS[UI_LIGHT]); }
  if (isRenaming) [[unlikely]] { DrawRename(ec, *this); }
}

void NodeGroup::update(EditorContext& ec) {
  const auto pressed = ec.input.isMBPressed(MOUSE_BUTTON_LEFT);

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
      for (auto* comp : node->components) {
        const float x = comp->x;
        comp->x = FLT_MAX;
        comp->update(ec, *node);
        comp->x = x;
      }
      // Node update after
      const float x = node->x;
      node->x = FLT_MAX;
      node->update(ec);
      node->x = x;
    }

    // Update used pins
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

void NodeGroup::addNode(EditorContext& ec, Node& node) {
  if (!std::ranges::contains(nodes, &node)) {
    nodes.push_back(&node);
    updateInternalState(ec);
  }
}

void NodeGroup::removeNode(EditorContext& ec, Node& node) {
  const int size = nodes.size();
  std::erase(nodes, &node);
  if (nodes.size() != size) {
    node.isInGroup = false;
    if (nodes.empty()) {
      std::erase(ec.core.nodeGroups, *this);
      return;
    }
    updateInternalState(ec);
  }
}

void NodeGroup::onConnectionAdded(EditorContext& ec, Node& node) {
  if (std::ranges::contains(nodes, &node)) { updateInternalState(ec); }
}

void NodeGroup::onNodeMoved(EditorContext& ec, Node& node, Rectangle nBounds) {
  const auto bounds = getBounds();
  // We dont care if node is in group and insdie us -> only add nodes that have no group
  const bool contained = CheckCollisionRecs(nBounds, bounds);
  const auto inGroup = node.isInGroup;
  if (inGroup && contained) return;

  // Our node has been moved out
  if (inGroup && std::ranges::contains(nodes, &node)) {
    removeNode(ec, node);
  } else if (!inGroup && contained) {
    addNode(ec, node);
  }
}

void NodeGroup::updateInternalState(const EditorContext& ec) {
  foldedDims.x = MeasureTextEx(ec.display.editorFont, name, ec.display.fontSize, 0.5F).x + 27;
  foldedDims.y = 100;
  dims.x = foldedDims.x;

  Rectangle bounding{};

  // Determine new bounds
  for (const auto n : nodes) {
    Display::ReduceToBounding(*n, bounding);
  }

  Display::ApplyInset(bounding, 35);

  pos = {bounding.x, bounding.y};
  dims = {bounding.width, bounding.height};

  usedPins.clear();

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

Rectangle NodeGroup::getBounds() const {
  if (expanded) return {pos.x, pos.y, dims.x, dims.y};
  return {pos.x, pos.y, static_cast<float>(foldedDims.x), static_cast<float>(foldedDims.y)};
}

// Invoke the event methods
void NodeGroup::InvokeConnection(EditorContext&ec, Node& node) {
  for(auto& g : ec.core.nodeGroups) {
    g.onConnectionAdded(ec, node);
  }
}

void NodeGroup::InvokeMoved(EditorContext& ec, Node& node) {
  const auto bounds = node.getBounds();
  for(auto& g : ec.core.nodeGroups) {
    g.onNodeMoved(ec, node,bounds);
  }
}

void NodeGroup::InvokeDelete(EditorContext& ec, Node& node) {
  for(auto& g : ec.core.nodeGroups) {
    g.removeNode(ec, node);
  }
}