#ifndef MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_
#define MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_

#include "shared/fwd.h"

#include <raylib.h>

class Window {
  inline static Vector2 dragOffset = {0.0f, 0.0f};
  Rectangle bodyRect;
  Rectangle headerRect;
  const char* headerText;
  bool isDragged = false;
  bool isWindowOpen = false;
  bool isHeaderHovered = false;
  WindowType type;

 public:
  Window(const Rectangle& bounds, WindowType type, const char* headerText)
      : bodyRect(bounds.x, bounds.y, bounds.width, bounds.height), headerRect(bounds.x, bounds.y, bounds.width, 20),
        headerText(headerText), type(type) {}
  virtual ~Window() = default;
  void draw(EditorContext& ec);
  // Opens the window
  void openWindow() noexcept;
  // Opens the window
  void closeWindow() noexcept;

  // Getters
  [[nodiscard]] WindowType getType() const { return type; }
  [[nodiscard]] auto isOpen() const -> bool { return isWindowOpen; }

 protected:
  // Events methods
  virtual void onOpen() {}
  virtual void onClose() {}
  virtual void drawContent(EditorContext& ec, const Rectangle& body) = 0;

 private:
  void update(EditorContext& ec);
};

#endif  //MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_