#ifndef RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_
#define RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_

#include <string>

struct TextAction;
struct TextInputField final : public Component {
  inline static uint8_t BLINK_DELAY = 45;
  std::string buffer;
  TextAction* currAction = nullptr;
  bool showCursor = false;
  bool isFocused = false;
  uint8_t cursorPos{};
  uint8_t blinkCounter = 0;
  explicit TextInputField(const char* name) : Component(name, 250, 20) {}
  Component* clone() final { return new TextInputField(*this); }
  void draw(float x, float y, EditorContext&) final;
  void update(float x, float y,EditorContext&) final;
  void load(FILE* file) final;
  void save(FILE* file) final;
  const char* getString() final { return buffer.c_str(); }
 private:
  void onFocusGain();
  void onFocusLoss(EditorContext& ec);
};

#endif  //RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_
