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
//

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

struct Display {
  Camera2D camera = {};
  Font editorFont = {};
  float fontSize = 17;
  Vector2 screenSize = {};
  float gridSpacing = 20;
  bool loadFont() {
    editorFont = LoadFont("res/monogram.ttf");
    return editorFont.texture.id != 0;
  }
  [[nodiscard]] Vector2 getAnchor(const AnchorPos a, const float relativeInset, const float w,
                                  const float h) const {
    Vector2 position;
    const float inset = getSpace(relativeInset);

    switch (a) {
      case LEFT_TOP:
        position = {inset, inset};
        break;
      case LEFT_CENTER:
        position = {inset, (screenSize.y - h) / 2};
        break;
      case LEFT_BOTTOM:
        position = {inset, screenSize.y - h - inset};
        break;
      case CENTER_TOP:
        position = {(screenSize.x - w) / 2, inset};
        break;
      case CENTER_MID:
        position = {(screenSize.x - w) / 2, (screenSize.y - h) / 2};
        break;
      case CENTER_BOTTOM:
        position = {(screenSize.x - w) / 2, screenSize.y - h - inset};
        break;
      case RIGHT_TOP:
        position = {screenSize.x - w - inset, inset};
        break;
      case RIGHT_MID:
        position = {screenSize.x - w - inset, (screenSize.y - h) / 2};
        break;
      case RIGHT_BOTTOM:
        position = {screenSize.x - w - inset, screenSize.y - h - inset};
        break;
      default:
        position = {inset, inset};
        break;
    }
    return position;
  }
  [[nodiscard]] float getSpace(const float size) const {
    const float minDimension = (screenSize.x < screenSize.y) ? screenSize.x : screenSize.y;
    return minDimension * size;
  }
};
#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_