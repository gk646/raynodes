#ifndef RAYNODES_SRC_SHARED_RAYUTILS_H_
#define RAYNODES_SRC_SHARED_RAYUTILS_H_

//#include <raylib.h> // raylib include is assumed / We don't force it
#include <cmath>

inline float GetDistManhattan(Vector2 a, Vector2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

#endif  //RAYNODES_SRC_SHARED_RAYUTILS_H_
