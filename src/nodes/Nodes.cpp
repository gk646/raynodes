#include "Nodes.h"

#include <ostream>
#include <cxutil/cxio.h>

#include "Components.h"
#include "shared/Types.h"
#include "application/EditorContext.h"
#include "application/elements/Action.h"


void Node::draw(EditorContext& ec) {
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  const auto rect = Rectangle{position.x, position.y, size.x, size.y};

  DrawRectanglePro(rect, {0, 0}, 0, BLACK);
  if (isHovered) {
    DrawRectangleLinesEx(rect, 2, ColorAlpha(YELLOW, 0.7));
  }
  DrawTextEx(font, NodeToString(type), {position.x + 5, position.y + 5}, fs, 1.0F, WHITE);

  float startX = position.x + 3;
  float startY = position.y + 20 + 3;
  for (auto c : components) {
    c->draw(startX, startY, ec);
    startY += c->getHeight();
  }

  size.y = (startY - position.y) + 10;
}

void Node::update(EditorContext& ec) {
  //Cache
  const auto rect = Rectangle{position.x, position.y, size.x, size.y};
  auto& selectedNodes = ec.core.selectedNodes;
  const auto worldMouse = ec.logic.worldMouse;

  //Update components
  float startX = position.x + 3;
  float startY = position.y + 20 + 3;

  for (auto c : components) {
    c->update(startX, startY, ec);
    if (c->getWidth() > size.x) {
      size.x = c->getWidth() + 10;
    }
    startY += c->getHeight();
  }

  if (ec.logic.isSelecting) {
    if (CheckCollisionRecs(rect, ec.logic.selectRect)) {
      selectedNodes.insert({id, this});
      isHovered = true;
    } else {
      isHovered = false;
    }
    //User is selecting // Don't update movement
    return;
  }

  //Another node is hovered no point in updating this one
  if (!isDragged && (ec.logic.isAnyNodeHovered || ec.logic.isDraggingNode)) {
    //Show as hovered when selected
    isHovered = selectedNodes.contains(id);
    return;
  }

  //Check if hovered
  isHovered = isDragged || CheckCollisionPointRec(worldMouse, rect);

  if (isHovered && !ec.logic.isAnyNodeHovered) {
    ec.logic.isAnyNodeHovered = true;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (!IsKeyDown(KEY_LEFT_CONTROL) && !selectedNodes.contains(id)) {
        selectedNodes.clear();
      }
      selectedNodes.insert({id, this});

      auto& moveAction = ec.logic.currentMoveAction;
      if (moveAction == nullptr) {
        moveAction = new NodeMovedAction((int)selectedNodes.size() + 1);
        for(auto pair : selectedNodes){
          moveAction->movedNodes.emplace_back(pair.first, pair.second->position);
        }
      }

      isDragged = true;
      DRAG_OFFSET = ec.logic.worldMouse;
      ec.logic.isDraggingNode = true;
      ec.logic.draggedNode = this;
    }

  } else {
    isHovered = !selectedNodes.empty() && selectedNodes.contains(id);
  }

  if (isDragged) {
    ec.logic.isAnyNodeHovered = true;
    ec.logic.draggedNode = this;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 movementDelta = {worldMouse.x - DRAG_OFFSET.x, worldMouse.y - DRAG_OFFSET.y};
      position.x += movementDelta.x;
      position.y += movementDelta.y;

      //Update selected nodes
      if (!selectedNodes.empty()) {
        for (auto pair : selectedNodes) {
          Node* node = pair.second;
          if (node != this) {  // Avoid moving this node again
            // Apply the same movement to all selected nodes
            node->position.x += movementDelta.x;
            node->position.y += movementDelta.y;
          }
        }
      }
      DRAG_OFFSET = worldMouse;
    } else {
      isDragged = false;
      ec.logic.isDraggingNode = false;
    }
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
  color = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b),
           static_cast<unsigned char>(a)};

  cxstructs::io_load(file, position.x);
  cxstructs::io_load(file, position.y);
  cxstructs::io_load(file, size.x);
  cxstructs::io_load(file, size.y);

  for (auto c : components) {
    c->load(file);
  }
}

void Node::addComponent(Component* comp) {
  components.push_back(comp);
}
Component* Node::getComponent(const char* name) {
  for (auto c : components) {
    if (c->name == name) return c;
  }
  for (auto c : components) {
    if (strcmp(name, c->name) == 0) return c;
  }
  return nullptr;
}
void HeaderNode::exportToMQQS(std::ostream& out) {
  out << "Name:" << getComponent("Description")->getString() << "\n";
  // out << "Description:" << description << "\n";
  // out << "Zone:" << zone << "\n";
  //  out << "Level:" << Level << "\n";
}
