#ifndef RAYNODES_SRC_NODES_COMPONENTS_H_
#define RAYNODES_SRC_NODES_COMPONENTS_H_

#include "shared/fwd.h"
#include <cstdio>

struct Component {
  const char* name;
  uint16_t width;
  uint16_t height;
  explicit Component(const char* name, uint16_t w = 0, uint16_t h = 0) : name(name), width(w), height(h) {}
  virtual Component* clone() = 0;
  virtual void draw(float x, float y, EditorContext&) = 0;
  virtual void update(float x, float y,EditorContext&) = 0;
  virtual void save(FILE* file) = 0;
  virtual void load(FILE* file) = 0;

  //Getters
  virtual const char* getString() { return nullptr; };
  virtual int getInt() { return 0; };
  virtual float getFloat() { return 0.0F; };
  virtual void* getData() { return nullptr; };

  [[nodiscard]] inline float getWidth() const { return width; }
  [[nodiscard]] inline float getHeight() const { return height; }
};

#include "elements/TextInputField.h"

#endif  //RAYNODES_SRC_NODES_COMPONENTS_H_
