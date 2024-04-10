#ifndef RAYNODES_SRC_NODES_COMPONENTS_H_
#define RAYNODES_SRC_NODES_COMPONENTS_H_

#include <string>

struct DrawResource;
struct UpdateResource;

struct Component {
  const char* name;
  uint16_t width;
  uint16_t height;
  explicit Component(const char* name, uint16_t w = 0, uint16_t h = 0) : name(name), width(w), height(h) {}
  virtual Component* clone() = 0;
  virtual void draw(float x, float y, DrawResource&) = 0;
  virtual void update(UpdateResource&) = 0;
  virtual void save(FILE* file) = 0;
  virtual void load(char** serializedState) = 0;

  //Getters
  virtual const char* getString() { return nullptr; };
  virtual int getInt() { return 0; };
  virtual float getFloat() { return 0.0F; };
  virtual void* getData() { return nullptr; };

  [[nodiscard]] inline float getWidth() const { return width; }
  [[nodiscard]] inline float getHeight() const { return height; }
};

struct TextInputField final : public Component {
  inline static uint8_t BLINK_DELAY = 45;
  std::string buffer;
  bool showCursor = false;
  bool isFocused = false;
  uint8_t cursorPos{};
  uint8_t blinkCounter = 0;
  explicit TextInputField(const char* name) : Component(name, 250, 20) {}
  Component* clone() final { return new TextInputField(*this); }
  void draw(float x, float y, DrawResource&) final;
  void update(UpdateResource&) final;
  void load(char** serializedState) final;
  void save(FILE* file) final;
  const char* getString() final { return buffer.c_str(); }
};
#endif  //RAYNODES_SRC_NODES_COMPONENTS_H_
