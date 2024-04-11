#include "Nodes.h"

#include <ostream>
#include <cxutil/cxio.h>

#include "shared/Types.h"
#include "Components.h"

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

void Node::draw(DrawResource& dr) {
  const auto font = dr.font;
  const auto fs = dr.fontSize;

  const auto rect = Rectangle{position.x, position.y, size.x, size.y};

  DrawRectanglePro(rect, {0, 0}, 0, BLACK);
  if (isHovered) {
    DrawRectangleLinesEx(rect, 2, ColorAlpha(YELLOW, 0.7));
  }
  DrawTextEx(font, NodeToString(type), {position.x + 5, position.y + 5}, fs, 1.0F, WHITE);

  float startX = position.x + 3;
  float startY = position.y + 20 + 3;
  for (auto c : components) {
    c->draw(startX, startY, dr);
    startY += c->getHeight();
  }

  size.y = (startY - position.y) + 10;
}

void Node::update(UpdateResource& ur) {
  //Update components
  for (auto c : components) {
    c->update(ur);
    if (c->getWidth() > size.x) {
      size.x = c->getWidth() + 10;
    }
  }

  const auto rect = Rectangle{position.x, position.y, size.x, size.y};

  if (ur.isSelecting) {
    if (CheckCollisionRecs(rect, ur.selectRect)) {
      ur.selectedNodes->insert({(uint16_t)id, this});
      isHovered = true;
    }
    //User is selecting // Don't update movement
    return;
  }

  //Another node is hovered no point in updating this one
  if (ur.isDraggingNode && !isHovered) {
    //Show as hovered when selected
    isHovered = ur.selectedNodes->contains((uint16_t)id);
    return;
  }


  //Check if hovered
  isHovered =  isDragged || CheckCollisionPointRec(ur.worldMouse, rect);



  if (isHovered) {
    ur.anyNodeHovered = true;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (IsKeyDown(KEY_LEFT_CONTROL)) {
        ur.selectedNodes->insert({(uint16_t)id, this});
      }

      if (!ur.selectedNodes->contains((uint16_t)id)) {
        ur.selectedNodes->clear();
      }

      isDragged = true;
      DRAG_OFFSET = ur.worldMouse;
      ur.isDraggingNode = true;
    }
  }

  if (isDragged) {
    ur.anyNodeHovered = true;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 movementDelta = {ur.worldMouse.x - DRAG_OFFSET.x, ur.worldMouse.y - DRAG_OFFSET.y};
      position.x += movementDelta.x;
      position.y += movementDelta.y;

      //Update selected nodes
      if (!ur.selectedNodes->empty()) {
        for (auto pair : *ur.selectedNodes) {
          Node* node = pair.second;
          if (node != this) {  // Avoid moving this node again
            // Apply the same movement to all selected nodes
            node->position.x += movementDelta.x;
            node->position.y += movementDelta.y;
          }
        }
      }
      DRAG_OFFSET = ur.worldMouse;
    } else {
      isDragged = false;
      ur.isDraggingNode = false;
    }
  }

  if (!isHovered) {
    isHovered = !ur.selectedNodes->empty() && ur.selectedNodes->contains((uint16_t)id);
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
