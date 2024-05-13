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

#ifndef RAYNODES_SRC_NODES_COMPONENT_H_
#define RAYNODES_SRC_NODES_COMPONENT_H_

#include "shared/fwd.h"

#include <cxstructs/StackVector.h>

#include "blocks/Pin.h"

#pragma warning(push)
#pragma warning(disable : 4100)  // unreferenced formal parameter

// ==============================
// COMPONENT INTERFACE
// ==============================

//Abstract interface to create dynamic components
// .....................................................................
//
// Rule 1: Stay within bounds and fit them to the component
// .....................................................................
//
// Rule 2: Use the provided input functions over raylib's:
//   - EditorContext::Input::isMousePressed(int key);
//   - if(ec.input.isMousePressed(MOUSE_BUTTON_LEFT){
// These functions respect the UI layers, otherwise you will get weird interactions
// .....................................................................
//
// Rule 3: You must use the provided io_save / io_load functions from cxutil/cxio.h
//    Otherwise correct persistence can not be guaranteed
// .....................................................................

struct Component {                                                   // Ordered after access pattern
  cxstructs::StackVector<OutputPin, OUTPUT_PINS, int8_t> outputs{};  // Current limit
  cxstructs::StackVector<InputPin, INPUT_PINS, int8_t> inputs{};     // Current limit
  float x = 0;                                                       // Internal state (don't change, only read)
  float y = 0;                                                       // Internal state (don't change, only read)
  uint16_t width = 50;                                               // Dynamically adjustable
  uint16_t height = 20;                                              // Dynamically adjustable
  bool isFocused = false;                                            // Internal state (don't change, only read)
  bool isHovered = false;                                            // Internal state (don't change, only read)
  bool internalLabel = true;                                         // Label drawn by the node or not
  const char* const label;                                           // Display name (and access name)
  const char* const id;                                              // Uniquely identifying id (allocated ptr)

  explicit Component(const ComponentTemplate ct, uint16_t w = 0, uint16_t h = 0)
      : width(w), height(h), label(ct.label), id(ct.component) {}
  virtual ~Component() = default;

  //-----------CORE-----------//
  // Necessary to copy the component
  virtual Component* clone() = 0;
  // IMPORTANT: Only called when its bounds are visible on the screen!
  virtual void draw(EditorContext& ec, Node& parent) = 0;
  // Guaranteed to be called once per tick (on the main thread) (not just when focused)
  virtual void update(EditorContext& ec, Node& parent) = 0;
  // Use the symmetric helpers : io_save(file,myFloat)...
  virtual void save(FILE* file) {}
  //Use the symmetric helpers : io_load(file,myFloat)...
  virtual void load(FILE* file) {}

  //-----------EVENTS-----------//
  // All called once, guaranteed before update() is called
  virtual void onMouseEnter(EditorContext& ec) {}
  virtual void onMouseLeave(EditorContext& ec) {}
  virtual void onFocusGain(EditorContext& ec) {}
  virtual void onFocusLoss(EditorContext& ec) {}

  //-----------LIFE CYCLE-----------//
  // Called once at creation time after the constructor
  virtual void onCreate(EditorContext& ec, Node& parent) {}
  // IMPORTANT: Only called once when the node is finally destroyed (only happens after its delete action is destroyed)
  // This may happen very delayed or even never!
  virtual void onDestruction(Node& parent) {}
  // Called whenever component is removed from the screen (delete/cut)
  virtual void onRemovedFromScreen(EditorContext& ec, Node& parent) {}
  // Called whenever component is added to the screen (paste)
  virtual void onAddedToScreen(EditorContext& ec, Node& parent) {}

  //-----------CONNECTIONS-----------//
  // Called once when a new connection is added
  virtual void onConnectionAdded(EditorContext& ec, const Connection& con) {}
  // Called once when an existing connection is removed
  virtual void onConnectionRemoved(EditorContext& ec, const Connection& con) {}

  //-----------PINS-----------//
  bool addPinInput(const PinType pt) {
    if (inputs.size() < inputs.capacity()) {
      inputs.push_back(InputPin(pt));
      return true;
    }
    return false;
  }
  bool addPinOutput(const PinType pt) {
    if (outputs.size() < outputs.capacity()) {
      outputs.push_back(OutputPin(pt));
      return true;
    }
    return false;
  }

  // Abstract data getters for external access
  virtual const char* getString() { return nullptr; }
  virtual int64_t getInt() { return 0; }
  virtual double getFloat() { return 0.0F; }
  virtual void* getData() { return nullptr; }
  virtual bool getBool() { return false; }

  // Getters
  [[nodiscard]] const char* getLabel() const { return label; }
  [[nodiscard]] float getWidth() const { return width; }
  [[nodiscard]] float getHeight() const { return height; }
  [[nodiscard]] Rectangle getBounds() const;
  [[nodiscard]] int getPinIndex(const Pin& p) const {
    if (p.direction == INPUT) {
      for (int i = 0; i < inputs.size(); i++) {
        if (&inputs[static_cast<int8_t>(i)] == &p) return i;
      }
      return -1;
    }
    for (int i = 0; i < outputs.size(); i++) {
      if (&outputs[static_cast<int8_t>(i)] == &p) return i;
    }
    return -1;
  }
};

#pragma warning(pop)

#endif  //RAYNODES_SRC_NODES_COMPONENT_H_