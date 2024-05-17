#ifndef MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_
#define MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_

#include "shared/fwd.h"

#include <raylib.h>

#pragma warning(push)
#pragma warning(disable : 4100)  // unreferenced formal parameter

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
  void openWindow(EditorContext& ec) noexcept;
  // Opens the window
  void closeWindow(EditorContext& ec) noexcept;
  void toggleWindow(EditorContext& ec) noexcept {
    if (isWindowOpen) closeWindow(ec);
    else openWindow(ec);
  }

  // Getters
  [[nodiscard]] WindowType getType() const { return type; }
  [[nodiscard]] auto isOpen() const -> bool { return isWindowOpen; }

 protected:
  // Events methods
  virtual void onOpen(EditorContext& ec) {}
  virtual void onClose(EditorContext& ec) {}
  virtual void drawContent(EditorContext& ec, const Rectangle& body) = 0;

 private:
  void update(EditorContext& ec);
};

#pragma warning(pop)

#endif  //MAGEQUEST_SRC_UI_COMPONENTS_GAMEWINDOW_H_