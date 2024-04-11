#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_

struct Display {
  Camera2D camera = {0};
  Font editorFont = {0};
  float fontSize = 16;
  Vector2 screenSize = {0};
  float gridSpacing = 20;

  inline bool loadFont() {
    editorFont = LoadFont("res/monogram.ttf");
    return editorFont.texture.id != 0;
  }
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTDISPLAY_H_
