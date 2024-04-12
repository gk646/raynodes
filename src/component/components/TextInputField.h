#ifndef RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_
#define RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_

#include "component/Component.h"

#include <string>

struct TextAction;

class TextInputField final : public Component {
  inline static uint8_t BLINK_DELAY = 45;
  std::string buffer;
  TextAction* currAction = nullptr;
  bool showCursor = false;
  uint8_t cursorPos{};
  uint8_t blinkCounter = 0;

 public:
  explicit TextInputField(const char* name) : Component(name, INPUT_FIELD_TEXT, 250, 20) {}
  Component* clone() final { return new TextInputField(*this); }
  void draw(float x, float y, EditorContext& ec, Node& parent) final;
  void update(float x, float y, EditorContext&, Node& parent) final;
  void load(FILE* file) final;
  void save(FILE* file) final;
  const char* getString() final { return buffer.c_str(); }
  void onFocusGain(EditorContext&) final;
  void onFocusLoss(EditorContext&) final;
};

#endif  //RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_
