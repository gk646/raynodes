#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_

enum AnchorPos : uint8_t
{
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

struct Display
{
  Camera2D camera = {0};
  Font editorFont = {0};
  float fontSize = 17;
  Vector2 screenSize = {0};
  float gridSpacing = 20;

  inline bool loadFont() {
    editorFont = LoadFont("res/monogram.ttf");
    return editorFont.texture.id != 0;
  }

  inline Vector2 getAnchor(AnchorPos a, float relativeInset, float w, float h) {
    Vector2 position;

    float inset = getSpace(relativeInset);

    switch (a)
      {
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

  float getSpace(float size) {
    float minDimension = (screenSize.x < screenSize.y) ? screenSize.x : screenSize.y;
    return minDimension * size;
  }
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_
