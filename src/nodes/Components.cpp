#include "Components.h"

#include <raylib.h>
#include <cxutil/cxmath.h>
#include <cxutil/cxio.h>

#include "shared/Types.h"

void TextInputField::draw(float x, float y, DrawResource& dr) {
  const auto rect = Rectangle{x, y, static_cast<float>(width), static_cast<float>(height)};
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isFocused = CheckCollisionPointRec(dr.worldMouse, rect);
    showCursor = isFocused;
    blinkCounter = 0;
  }

  const auto& font = dr.font;
  constexpr float fs = 15;
  const auto drawY = y + (height - fs) / 2.3F;

  const auto prefixWidth = MeasureTextEx(font, name, fs, 1.0F).x;
  auto drawX = x - (prefixWidth + 3);
  DrawTextEx(font, name, {drawX, drawY}, fs, 1.0, DARKGRAY);

  DrawRectangleRec(rect, isFocused ? DARKGRAY : GRAY);
  DrawRectangleLinesEx(rect, 1, BLACK);

  // Clamp the cursor position
  cursorPos = cxstructs::clamp((int)cursorPos, 0, static_cast<int>(buffer.size()));
  DrawTextEx(font, buffer.c_str(), {x + 3, drawY}, fs, 1.0F, WHITE);
  const auto c = buffer[cursorPos];
  buffer[cursorPos] = '\0';

  const auto beforeWidth = MeasureTextEx(font, buffer.c_str(), fs, 1.0F).x;
  buffer[cursorPos] = c;
  if (showCursor) {
    DrawTextEx(font, "|", {x + beforeWidth, drawY}, fs + 2, 1.0F, WHITE);
  } else {
    DrawTextEx(font, " ", {x + beforeWidth, drawY}, fs + 2, 1.0F, WHITE);
  }
}

void TextInputField::update(UpdateResource& ur) {
  if (isFocused && IsKeyPressed(KEY_ESCAPE)) {
    isFocused = false;
    // ConsumeKeyState(); //Non existent in standard raylib
  }

  if (!isFocused) return;

  int key = GetCharPressed();
  while (key > 0) {
    if ((key >= 32) && (key <= 125) && (buffer.length() < 1024)) {
      buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
      cursorPos++;
    }

    key = GetCharPressed();  // Check next character in the queue
  }

  if (cursorPos > 0 && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))) {
    buffer.erase(buffer.begin() + cursorPos - 1);
    cursorPos--;
  }

  if (cursorPos < buffer.size() && (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE))) {
    buffer.erase(buffer.begin() + cursorPos);
  }

  if (cursorPos < buffer.length() && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))) {
    cursorPos++;
  }

  if (cursorPos > 0 && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))) {
    cursorPos--;
  }

  // Handle blinking cursor
  blinkCounter++;
  if (blinkCounter >= BLINK_DELAY) {
    showCursor = !showCursor;
    blinkCounter = 0;
  }
}

void TextInputField::save(FILE* file) {
  cxstructs::io_save(file, buffer.c_str());
}
void TextInputField::load(FILE* file) {
  cxstructs::io_load(file, buffer);
}