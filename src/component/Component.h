#ifndef RAYNODES_SRC_NODES_COMPONENT_H_
#define RAYNODES_SRC_NODES_COMPONENT_H_

#include "shared/fwd.h"
#include <cstdio>

struct Component {
  const char* name;
  uint16_t width;
  uint16_t height;
  const ComponentType type;
  bool isFocused = false;
  bool isHovered = false;

  explicit Component(const char* name, ComponentType type, uint16_t w = 0, uint16_t h = 0)
      : name(name), width(w), height(h), type(type) {}
  virtual ~Component() = default;

  //-----------CORE-----------//
  //Necessary to copy the component
  virtual Component* clone() = 0;
  virtual void draw(float x, float y, EditorContext& ec, Node& parent) = 0;
  virtual void update(float x, float y, EditorContext& ec, Node& parent) = 0;
  //Use the symmetric helpers : cx_save(file,myFloat)...
  virtual void save(FILE* file) = 0;
  //Use the symmetric helpers : cx_load(file,myFloat)...
  virtual void load(FILE* file) = 0;

  //-----------EVENTS-----------//
  // All called once, guaranteed before update() is called
  virtual void onMouseEnter(EditorContext& ec) {}
  virtual void onMouseLeave(EditorContext& ec) {}
  virtual void onFocusGain(EditorContext& ec) {}
  virtual void onFocusLoss(EditorContext& ec) {}

  //-----------LIFE CYCLE-----------//
  //Called once at creation time
  virtual void onCreate(EditorContext& ec, Node& parent) {}
  //IMPORTANT: Only called once when the node is finally destroyed (only happens after its delete action is destroyed)
  //This may happen very delayed or even never!
  virtual void onDestruction(Node& parent) {}
  //Called whenever component is removed from the screen (delete/cut)
  virtual void onRemovedFromScreen(EditorContext& ec, Node& parent) {}
  //Called whenever component is added to the screen (paste)
  virtual void onAddedToScreen(EditorContext& ec, Node& parent) {}

  //-----------CONNECTIONS-----------//
  //Called once when a new connection is added
  virtual void onConnectionAdded(EditorContext& ec, const Connection& con) {}
  //Called once when an existing connection is removed
  virtual void onConnectionRemoved(EditorContext& ec, const Connection& con) {}

  // Abstract data getters for external access
  virtual const char* getString() { return nullptr; }
  virtual int getInt() { return 0; }
  virtual float getFloat() { return 0.0F; }
  virtual void* getData() { return nullptr; }

  // Default getters
  [[nodiscard]] inline float getWidth() const { return width; }
  [[nodiscard]] inline float getHeight() const { return height; }
  [[nodiscard]] inline const char* getName() const { return name; }
};

enum ComponentType : uint8_t {
  INPUT_FIELD_TEXT,
  INPUT_FIELD_TEXT_MULTILINE,
  INPUT_FIELD_INTEGER,
  INPUT_FIELD_BOOLEAN,
  INPUT_FIELD_FLOAT,
};
#endif  //RAYNODES_SRC_NODES_COMPONENT_H_
