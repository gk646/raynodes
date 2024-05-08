// Copyright (c) 2024 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_

enum AnchorPos : uint8_t {
  LEFT_TOP,
  LEFT_CENTER,
  LEFT_BOTTOM,
  CENTER_TOP,
  CENTER_MID,
  CENTER_BOTTOM,
  RIGHT_TOP,
  RIGHT_MID,
  RIGHT_BOTTOM
};

struct Display final {
  static constexpr float MAX_ZOOM = 3.0F;
  static constexpr float MIN_ZOOM = 0.1F;

  RenderTexture uiTexture;
  Font editorFont = {};
  Camera2D camera = {};
  Vector2 screenSize = {};
  float fontSize = 17.0F;
  float gridSpacing = 20.0F;

  void zoomIn() { camera.zoom = std::min(MAX_ZOOM, camera.zoom + MAX_ZOOM / 10.0F); }
  void zoomOut() { camera.zoom = std::max(MIN_ZOOM, camera.zoom - MAX_ZOOM / 10.0F); }

  // Getters
  [[nodiscard]] auto getAnchor(AnchorPos a, float relativeInset, float w, float h) const -> Vector2 {
    Vector2 position;
    const float inset = relativeInset == 0 ? 0 : getSpace(relativeInset);

    switch (a) {
      case LEFT_TOP:
        position = {inset, inset};
        break;
      case LEFT_CENTER:
        position = {inset, (UI::UI_SPACE_H - h) / 2};
        break;
      case LEFT_BOTTOM:
        position = {inset, UI::UI_SPACE_H - h - inset};
        break;
      case CENTER_TOP:
        position = {(UI::UI_SPACE_W - w) / 2, inset};
        break;
      case CENTER_MID:
        position = {(UI::UI_SPACE_W - w) / 2, (UI::UI_SPACE_H - h) / 2};
        break;
      case CENTER_BOTTOM:
        position = {(UI::UI_SPACE_W - w) / 2, UI::UI_SPACE_H - h - inset};
        break;
      case RIGHT_TOP:
        position = {UI::UI_SPACE_W - w - inset, inset};
        break;
      case RIGHT_MID:
        position = {UI::UI_SPACE_W - w - inset, (UI::UI_SPACE_H - h) / 2};
        break;
      case RIGHT_BOTTOM:
        position = {UI::UI_SPACE_W - w - inset, UI::UI_SPACE_H - h - inset};
        break;
      default:
        position = {inset, inset};
        break;
    }
    return position;
  }
  [[nodiscard]] auto getFontSize(float scale) const -> float { return std::max(fontSize, getSpace(scale)); }
  [[nodiscard]] auto getSpace(const float size) const -> float {
    const float minDimension = (screenSize.x < screenSize.y) ? screenSize.x : screenSize.y;
    return minDimension * size;
  }
  // Only applies scaling when screen is bigger than the ui space -> elements dont shrink
  [[nodiscard]] auto getSmartScaled(const Rectangle& bounds) const -> Rectangle {
    Rectangle ret;

    ret.width = bounds.width;
    ret.height = bounds.height;

    const float scaleX = screenSize.x / UI::UI_SPACE_W;
    if (bounds.x > screenSize.x) {
      ret.x = bounds.x * scaleX;
    } else {
      ret.x = bounds.x;
    }

    if (bounds.y > screenSize.y) {
      const float scaleY = screenSize.y / UI::UI_SPACE_H;
      ret.y = bounds.y * scaleY;

    } else {
      ret.y = bounds.y;
    }

    if (scaleX > 1.0F) ret.width *= scaleX, ret.height *= scaleX;
    return ret;
  }
  // Always scales to the current screen space
  [[nodiscard]] auto getFullyScaled(const Rectangle& bounds) const -> Rectangle {
    Rectangle ret;
    const float scaleX = screenSize.x / UI::UI_SPACE_W;
    const float scaleY = screenSize.y / UI::UI_SPACE_H;
    ret.x = bounds.x * scaleX;
    ret.y = bounds.y * scaleY;
    ret.width = bounds.width * scaleX;
    ret.height = bounds.height * scaleY;
    return ret;
  }

  bool loadResources(EditorContext& ec);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_