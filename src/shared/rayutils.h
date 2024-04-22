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

inline bool CheckPointInsideRectangle(const Vector2 point, const Rectangle& rect) {
  return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y
         && point.y <= rect.y + rect.height;
}
inline float EaseCubicIn(float t, float b, float c, float d) {
  if ((t /= 0.5f * d) < 1) return 0.5f * c * t * t * t + b;

  t -= 2;

  return 0.5f * c * (t * t * t + 2.0f) + b;
}
inline bool CheckCollisionBezierRect(const Vector2 startPos, const Vector2 endPos, const Rectangle& rect,
                                      int numPoints = 100) {
  Vector2 current;
  for (int i = 0; i <= numPoints; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(numPoints);
    current.x = startPos.x + t * (endPos.x - startPos.x);
    current.y = EaseCubicIn(t * numPoints, startPos.y, endPos.y - startPos.y, numPoints);

    if (CheckPointInsideRectangle(current, rect)) {
      return true;
    }
  }
  return false;
}
#endif  //RAYNODES_SRC_SHARED_RAYUTILS_H_