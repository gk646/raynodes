#include "nodes/Components.h"

#include <cxutil/cxmath.h>
#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

void TextInputField::draw(float x, float y, EditorContext& ec) {
  const auto rect = Rectangle{x, y, (float)width, (float)height};

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      bool wasFocused = isFocused;

      //User clicked on textfield
      if (CheckCollisionPointRec(ec.logic.worldMouse, rect))
        {
          isFocused = true;
          blinkCounter = 0;
          showCursor = true;
        }
      else
        {
          isFocused = false;
          showCursor = false;
        }

      if (wasFocused != isFocused)
        {
          if (isFocused)
            {
              onFocusGain();
            }
          else
            {
              onFocusLoss(ec);
            }
        }
    }

  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto drawY = y + (height - fs) / 2.3F;
  const auto prefixWidth = MeasureTextEx(font, name, fs, 1.0F).x;
  cursorPos = cxstructs::clamp((int)cursorPos, 0, static_cast<int>(buffer.size()));

  //Measure cursor position
  char charAtCursor = buffer[cursorPos];
  buffer[cursorPos] = '\0';
  const auto beforeWidth = MeasureTextEx(font, buffer.c_str(), fs, 1.0F).x;
  buffer[cursorPos] = charAtCursor;
  auto drawX = x - (prefixWidth + 3);

  //Label text
  DrawTextEx(font, name, {drawX, drawY}, fs, 1.0, DARKGRAY);
  //Main field
  DrawRectangleRec(rect, isFocused ? DARKGRAY : GRAY);
  //Draw the text
  DrawTextEx(font, buffer.c_str(), {x + 3, drawY}, fs, 1.0F, WHITE);
  //Draw cursor
  if (showCursor)
    {
      DrawTextEx(font, "|", {x + beforeWidth, drawY}, fs + 2, 1.0F, WHITE);
    }
}

void TextInputField::update(float x, float y, EditorContext& ec) {
  if (isFocused && IsKeyPressed(KEY_ESCAPE))
    {
      isFocused = false;
      onFocusLoss(ec);
      // ConsumeKeyState(); //Non existent in standard raylib
    }

  if (!isFocused) return;

  int key = GetCharPressed();
  while (key > 0)
    {
      if ((key >= 32) && (key <= 125) && (buffer.length() < 1024))
        {
          buffer.insert(buffer.begin() + cursorPos, static_cast<char>(key));
          cursorPos++;
        }

      key = GetCharPressed();  // Check next character in the queue
    }

  if (cursorPos > 0 && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)))
    {
      buffer.erase(buffer.begin() + cursorPos - 1);
      cursorPos--;
    }

  if (cursorPos < buffer.size() && (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)))
    {
      buffer.erase(buffer.begin() + cursorPos);
    }

  if (cursorPos < buffer.length() && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)))
    {
      cursorPos++;
    }

  if (cursorPos > 0 && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)))
    {
      cursorPos--;
    }

  // Handle blinking cursor
  blinkCounter++;
  if (blinkCounter >= BLINK_DELAY)
    {
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

void TextInputField::onFocusGain() {
  if (currAction)
    {
      delete currAction;
      currAction = nullptr;
    }
  currAction = new TextAction(buffer, buffer);
}

void TextInputField::onFocusLoss(EditorContext& ec) {
  if (currAction)
    {
      //TRANSFER OWNERSHIP
      if (currAction->beforeState == buffer)
        {
          delete currAction;
          currAction = nullptr;
          return;
        }
      currAction->setAfter(buffer);
      ec.core.addEditorAction(currAction);
      currAction = nullptr;
    }
}