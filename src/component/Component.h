#ifndef RAYNODES_SRC_NODES_COMPONENT_H_
#define RAYNODES_SRC_NODES_COMPONENT_H_

#include "shared/fwd.h"

#include <cstdio>
#include <cxstructs/StackVector.h>

#include "blocks/Pin.h"

//Abstract interface to create dynamic components
//Rule 1: Stay within bounds and fit them to the component
//
//Rule 2: Use the provided input functions over raylib's:
//  - EditorContext::Input::isMouse___(int key);
//  - if(ec.input.isMousePressed(MOUSE_BUTTON_LEFT){
//
//  These functions respect the ui layers, otherwise you will get weird interactions

struct Component {
  cxstructs::StackVector<Pin, 5> inputs{};
  cxstructs::StackVector<Pin, 5> outputs{};
  const char* const name;
  float x = 0;           //Internal state (don't change, only read)
  float y = 0;           //Internal state (don't change, only read)
  uint16_t width = 50;   //Dynamically adjustable
  uint16_t height = 20;  //Dynamically adjustable
  const ComponentType type;
  bool isFocused = false;     //Internal state (don't change, only read)
  bool isHovered = false;     //Internal state (don't change, only read)
  bool internalLabel = true;  //If the label should be drawn or the component handles it

  explicit Component(const char* name, const ComponentType type, const uint16_t w = 0, const uint16_t h = 0)
      : name(name), width(w), height(h), type(type) {}
  virtual ~Component() = default;

  //-----------CORE-----------//
  //Necessary to copy the component
  virtual Component* clone() = 0;
  //Called once each tick (on the main thread)
  virtual void draw(EditorContext& ec, Node& parent) = 0;
  //Guaranteed to be called once per tick (on the main thread) (not just when focused)
  virtual void update(EditorContext& ec, Node& parent) = 0;
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
  //Called once at creation time after the constructor
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

  // Getters
  [[nodiscard]] const char* getName() const { return name; }
  [[nodiscard]] float getWidth() const { return width; }
  [[nodiscard]] float getHeight() const { return height; }
  [[nodiscard]] Rectangle getBounds() const;
};

enum ComponentType : uint8_t {
  INPUT_FIELD_TEXT,
  INPUT_FIELD_INTEGER,
  INPUT_FIELD_BOOLEAN,
  INPUT_FIELD_FLOAT,
  MATH_OPERATION,
  STRING_TO_NUMBER,
};

#endif  //RAYNODES_SRC_NODES_COMPONENT_H_