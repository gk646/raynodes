#include "Window.h"

#include "application/EditorContext.h"
#include "shared/rayutils.h"

void Window::draw(EditorContext& ec) {
  if (!isWindowOpen) return;

  update(ec);

  const Rectangle body = ec.display.getFullyScaled(bodyRect);
  const Rectangle header = ec.display.getFullyScaled(headerRect);

  Color headerColor;

  if (isDragged) {
    headerColor = UI::COLORS[UI_DARK];
  } else if (isHeaderHovered) {
    headerColor = UI::Darken(UI::COLORS[UI_MEDIUM]);
  } else {
    headerColor = UI::COLORS[UI_MEDIUM];
  }

  // Body and header
  DrawRectangleRec(body, UI::COLORS[UI_MEDIUM]);
  DrawRectangleRec(header, headerColor);

  DrawRectangleLinesEx(body, 2, UI::COLORS[UI_DARK]);
  DrawRectangleLinesEx(header, 2, UI::COLORS[UI_DARK]);

  // Close button
  {
    const auto closeColor = isHeaderHovered || isDragged ? UI::COLORS[UI_LIGHT] : UI::COLORS[UI_DARK];
    const float closeSize = header.height * 0.8f;
    const float closePadding = (header.height - closeSize) / 2;
    const Rectangle cBounds = {header.x + header.width - closeSize - closePadding, header.y + closePadding,
                               closeSize, closeSize};
    DrawRectangleLinesEx(cBounds, 1, closeColor);

    const Vector2 closeTopLeft = {cBounds.x, cBounds.y};
    const Vector2 closeBottomRight = {cBounds.x + cBounds.width, cBounds.y + cBounds.height};
    DrawLineEx(closeTopLeft, closeBottomRight, 1, closeColor);
    const Vector2 closeTopRight = {cBounds.x + cBounds.width, cBounds.y};
    const Vector2 closeBottomLeft = {cBounds.x, cBounds.y + cBounds.height};
    DrawLineEx(closeTopRight, closeBottomLeft, 1, closeColor);

    if (CheckCollisionPointRec(ec.logic.mouse, cBounds) && ec.input.isMBReleased(MOUSE_BUTTON_LEFT)) {
      closeWindow(ec);
    }
  }

  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto textPos = Vector2{body.x + body.width / 2, body.y};
  const auto textColor = isHeaderHovered || isDragged ? UI::COLORS[UI_LIGHT] : UI::COLORS[UI_DARK];
  DrawCenteredText(font, headerText, textPos, fs, 1.0F, textColor);

  drawContent(ec, {bodyRect.x, bodyRect.y + 20, bodyRect.width, bodyRect.height - 20});

  if (CheckCollisionPointRec(ec.logic.mouse, body)) ec.input.consumeMouse();
}

void Window::update(EditorContext& ec) {
  const auto header = ec.display.getFullyScaled(headerRect);
  const auto mousePos = ec.logic.mouse;

  // Calculate scaling factors based on the UI design resolution
  const float scaleX = UI::UI_SPACE_W / ec.display.screenSize.x;
  const float scaleY = UI::UI_SPACE_H / ec.display.screenSize.y;

  if (isDragged && ec.input.isMBDown(MOUSE_BUTTON_LEFT)) {
    isHeaderHovered = true;
    ec.input.consumeMouse();

    // Calculate the movement delta in screen space, then scale it to UI space
    const auto dx = (mousePos.x - dragOffset.x) * scaleX;
    const auto dy = (mousePos.y - dragOffset.y) * scaleY;

    // Update positions in UI space
    bodyRect.x += dx;
    bodyRect.y += dy;
    headerRect.x += dx;
    headerRect.y += dy;

    // Update the drag offset for the next frame
    dragOffset.x = mousePos.x;
    dragOffset.y = mousePos.y;
  } else {
    isDragged = false;
  }

  if (CheckCollisionPointRec(mousePos, header)) {
    isHeaderHovered = true;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isDragged) {
      ec.input.consumeMouse();
      dragOffset.x = mousePos.x;
      dragOffset.y = mousePos.y;
      isDragged = true;
    }
  } else {
    isHeaderHovered = false;
  }
  if (ec.input.isKeyReleased(KEY_ESCAPE)) closeWindow(ec);
}

void Window::openWindow(EditorContext& ec) noexcept {
  if (isWindowOpen) return;
  isWindowOpen = true;
  isDragged = false;
  onOpen(ec);
}

void Window::closeWindow(EditorContext& ec) noexcept {
  if (!isWindowOpen) return;
  isWindowOpen = false;
  isDragged = false;
  onClose(ec);
}