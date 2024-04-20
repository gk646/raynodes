#ifndef RAYNODES_SRC_SHARED_RAYUTILS_H_
#define RAYNODES_SRC_SHARED_RAYUTILS_H_

//#include <raylib.h> // raylib include is assumed / We don't force it
#include <cmath>

inline float GetDistManhattan(const Vector2 a, const Vector2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

//Measure the given text up to the given index
//Does not do any bound checks
inline float MeasureTextUpTo(char* text, int idx, const Font& f, const float fs, const float spacing = 1.0F) {
  const auto temp = text[idx];
  text[idx] = '\0';
  const float ret = MeasureTextEx(f, text, fs, spacing).x;
  text[idx] = temp;
  return ret;
}

//Horizontally centers the text
inline void DrawCenteredText(const Font& f, const char* txt, const Vector2 pos, float fs, float spc,
                             Color c) {
  const auto width = MeasureTextEx(f, txt, fs, spc).x;
  DrawTextEx(f, txt, {pos.x - width / 2.0F, pos.y}, fs, spc, c);
}

#endif  //RAYNODES_SRC_SHARED_RAYUTILS_H_