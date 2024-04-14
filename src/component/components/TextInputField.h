#ifndef RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_
#define RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_

#include "component/Component.h"

#include <string>

struct TextAction;
struct Font;
class TextInputField final : public Component {
  inline static uint8_t BLINK_DELAY = 45;
  std::string buffer;
  TextAction* currAction = nullptr;
  uint16_t cursorPos{};
  uint16_t selectionStart = 0;
  uint16_t selectionEnd = 0;
  bool isDragging = false;
  bool showCursor = false;
  uint8_t blinkCounter = 0;

 public:
  explicit TextInputField(const char* name) : Component(name, INPUT_FIELD_TEXT, 250, 20) {}
  Component* clone() override { return new TextInputField(*this); }
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext&, Node& parent) override;
  void load(FILE* file) override;
  void save(FILE* file) override;
  void onFocusGain(EditorContext&) override;
  void onFocusLoss(EditorContext&) override;
  void onCreate(EditorContext& ec, Node& parent) override;
  const char* getString() override { return buffer.c_str(); }

 private:
  [[nodiscard]] uint16_t getIndexFromPos(const Font& font, float fs, Vector2 mouse) const;
};

#endif  //RAYNODES_SRC_NODES_ELEMENTS_TEXTINPUTFIELD_H_