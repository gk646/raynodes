#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "shared/fwd.h"

#include <raylib.h>
#include <string>

enum InputConstraint {
  NONE,
  NUMERIC,
};

struct TextInputField final {
  inline static uint8_t BLINK_DELAY = 45;

  std::string buffer;
  Rectangle bounds;
  const Font* font = nullptr;
  float fs = 15.0F;  // Font size
  uint16_t cursorPos = 0;
  uint16_t selectionStart = 0;
  uint16_t selectionEnd = 0;
  bool isDragging = false;
  bool showCursor = false;
  bool isFocused = false;
  bool growAutomatic = true;
  uint8_t blinkCounter = 0;
  const InputConstraint constraint;

  explicit TextInputField(float w, float h, InputConstraint constraint)
      : bounds(0, 0, w, h), font(font), constraint(constraint) {}
  void draw();
  void update(Vector2 mouse);
  void onFocusGain(Vector2 mouse);
  void onFocusLoss();

 private:
  void updateDimensions(Rectangle& bounds);
  [[nodiscard]] Ints getSelection() const;
  [[nodiscard]] uint16_t getIndexFromPos(Vector2 mouse);
};

#endif  //TEXTFIELD_H