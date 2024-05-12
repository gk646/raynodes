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
#ifndef CX_TIME_H
#  define CX_TIME_H

#  include "../cxconfig.h"
#  include <chrono>
#  include <print>

namespace cxstructs {
using namespace std;  //std:: makes this code unreadable
inline static constexpr int MAX_CHECKPOINTS = 4;
inline chrono::time_point<chrono::high_resolution_clock> checkpoints[MAX_CHECKPOINTS];

/**
 * Sets the activeTimeStamp or alternatively the time of a checkpoint
 * @param checkpoint (optional, max=2) the checkpoint to set the current time
 */
inline void now(const int checkpoint = 0) {
  checkpoints[checkpoint] = std::chrono::high_resolution_clock::now();
}

template <typename T>
auto get_duration_unit() -> const char*;
template <>
inline auto get_duration_unit<std::chrono::seconds>() -> const char* {
  return "seconds";
}
template <>
inline auto get_duration_unit<std::chrono::milliseconds>() -> const char* {
  return "milliseconds";
}
template <>
inline auto get_duration_unit<std::chrono::microseconds>() -> const char* {
  return "microseconds";
}
template <>
inline auto get_duration_unit<std::chrono::nanoseconds>() -> const char* {
  return "nanoseconds";
}
template <>
inline auto get_duration_unit<std::chrono::duration<double>>() -> const char* {
  return "seconds";
}

template <typename DurationType = std::chrono::duration<double>>
void printTime(const char* prefix = nullptr, const int checkpoint = 0) {
  const auto diff = std::chrono::high_resolution_clock::now() - checkpoints[checkpoint];
  const auto diffInDesiredUnits = std::chrono::duration_cast<DurationType>(diff);

  if (prefix) {
#  if _HAS_CXX23
    std::print("{} ", prefix);
#  else
    printf("%s", prefix);
#  endif
  }
#  if _HAS_CXX23
  std::print("{} {}\n", diffInDesiredUnits.count(), get_duration_unit<DurationType>());
#  else
  printf("%lld %s\n", diffInDesiredUnits.count(), get_duration_unit<DurationType>());
#  endif
}
template <typename DurationType = std::chrono::duration<double>>
auto getTime(const int checkpoint = 0) -> long long {
  const auto diff = std::chrono::high_resolution_clock::now() - checkpoints[checkpoint];
  auto diffInDesiredUnits = std::chrono::duration_cast<DurationType>(diff);
  return diffInDesiredUnits.count();
}

}  // namespace cxstructs
#endif  //CX_TIME_H
