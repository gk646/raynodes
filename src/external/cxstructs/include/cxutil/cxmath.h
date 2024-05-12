// Copyright (c) 2023 gk646
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
#define CX_FINISHED
#ifndef CXSTRUCTS_SRC_CXUTIL_MATH_H_
#  define CXSTRUCTS_SRC_CXUTIL_MATH_H_

#  include <cmath>

namespace cxstructs {
// for compatibility | apparently this is only in c++ through std::numbers which is CX20 and not on all compilers equal
#  define CX_PI 3.14159265358979323846F
#  define CX_DEG2RAD (PI / 180.0F)
#  define CX_RAD2DEG (180.0F / PI)

struct mat;
//function pointer typedef
using func = float (*)(float);
using func_M = mat (*)(mat&, mat&);  // mat function
using D_func = float (*)(float, float, float, float);

//activation functions
inline auto sig(float x) noexcept -> float {
  return 1.0F / (1.0F + std::exp(-x));
}
inline auto tanh(float x) noexcept -> float {
  return std::tanh(x);
}
inline auto relu(float x) noexcept -> float {
  return x > 0 ? x : 0;
}
//derivatives
inline auto d_sig(float x) noexcept -> float {
  return sig(x) * (1 - sig(x));
}
inline auto d_relu(float x) noexcept -> float {
  return x > 0 ? 1.0F : 0.0F;
}
inline auto d_tanh(float x) noexcept -> float {
  float t = std::tanh(x);
  return 1 - t * t;
}

// Finds the closest power of two to the right of the given number
inline auto next_power_of_2(uint32_t n) noexcept -> uint32_t {
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}
// Fast inverse square root from quake (inversed)
inline auto fast_sqrt(float n) noexcept -> float {
  long i;
  float x2, y;
  constexpr float threehalfs = 1.5F;

  x2 = n * 0.5F;
  y = n;
  i = *(long*)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*)&i;
  y = y * (threehalfs - (x2 * y * y));
  return 1.0F / y;
}
// Clamps the given value between the ]range - low[ if val is smaller than low, val if val is between low and high, and high if val is bigger than high
template <typename T>
constexpr auto clamp(const T& val, const T& low, const T& high) -> T
  requires(!std::is_integral_v<T>)
{
  if (val < low) {
    return low;
  }
  if (val > high) {
    return high;
  }

  return val;
}
// Clamps the given value between the ]range - low[ if val is smaller than low, val if val is between low and high, and high if val is bigger than high
template <typename T>
constexpr auto clamp(T val, T low, T high) -> T
  requires std::is_integral_v<T>
{
  if (val < low) {
    return low;
  }
  if (val > high) {
    return high;
  }
  return val;
}
// Linear interpolation - translates the given old value in the old scaling to the corresponding point in the new scale
template <typename T, typename U>
T lerp(T valueOld, T minOld, T maxOld, U minNew, U maxNew) {
  return (static_cast<double>(valueOld - minOld) / static_cast<double>(maxOld - minOld)) * (maxNew - minNew) + minNew;
}
// True if the given "val" is within the specified range
template <typename T>
bool in_range(T val, T min, T max) {
  return val > min && val < max;
}

//-----------DISTANCE-----------//
inline auto euclidean(float p1x, float p1y, float p2x, float p2y) noexcept -> float {
  return fast_sqrt((p2x - p1x) * (p2x - p1x) + (p2y - p1y) * (p2y - p1y));
}
inline auto manhattan(float p1x, float p1y, float p2x, float p2y) noexcept -> float {
  return abs(p2x - p1x) + abs(p2y - p1y);
}

}  // namespace cxstructs
#endif  //CXSTRUCTS_SRC_CXUTIL_MATH_H_