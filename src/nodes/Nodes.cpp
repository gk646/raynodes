#include "Nodes.h"

#include <raygui.h>
#include <persist/PropertySaver.h>
#include <graphics/ComponentRenderer.h>
#include "Components.h"

void Node::saveState(FILE* file) {
  addSaveProperty(file, static_cast<int>(type));
  addSaveProperty(file, color.r);
  addSaveProperty(file, color.g);
  addSaveProperty(file, color.b);
  addSaveProperty(file, color.a);
  addSaveProperty(file, position.x);
  addSaveProperty(file, position.y);
  addSaveProperty(file, size.x);
  addSaveProperty(file, size.y);

  for (auto c : components) {
    c->save(file);
  }
}

void Node::loadState(char** serializedState) {
  int typeInt;
  loadProperty(serializedState, typeInt);
  type = static_cast<NodeType>(typeInt);

  int r, g, b, a;
  loadProperty(serializedState, r);
  loadProperty(serializedState, g);
  loadProperty(serializedState, b);
  loadProperty(serializedState, a);
  color = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b),
           static_cast<unsigned char>(a)};

  loadProperty(serializedState, position.x);
  loadProperty(serializedState, position.y);

  loadProperty(serializedState, size.x);
  loadProperty(serializedState, size.y);

  for (auto c : components) {
    c->load(serializedState);
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
  for (auto c : components) {
    c->update(ur);
    if (c->getWidth() > size.x) {
      size.x = c->getWidth() + 10;
    }
  }

  const auto rect = Rectangle{position.x, position.y, size.x, size.y};

  isHovered = CheckCollisionPointRec(ur.worldMouse, rect);

  if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isDragged = true;
    DRAG_OFFSET = {ur.worldMouse.x - position.x, ur.worldMouse.y - position.y};
  }

  if (isDragged) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      position.x = ur.worldMouse.x - DRAG_OFFSET.x;
      position.y = ur.worldMouse.y - DRAG_OFFSET.y;
    } else {
      isDragged = false;
    }
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
