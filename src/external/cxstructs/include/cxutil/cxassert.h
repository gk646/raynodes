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
#ifndef CXSTRUCTS_SRC_CXASSERT_H_
#  define CXSTRUCTS_SRC_CXASSERT_H_

#  include <cstdio>

inline void CX_ASSERT_failed(const char* expr, const char* file, int line, const char* message) {
  fprintf(stderr, "Assert failed: %s\nAt: %s:%d\nMessage: %s", expr, file, line, message);
#  if defined(_MSC_VER)
  __debugbreak();
#  elif defined(__GNUC__)
  __builtin_trap();
#  else
  std::abort();
#  endif
}

inline void WARNING_failed(const char* expr, const char* file, int line, const char* message) {
  fprintf(stderr, "Warning: %s\nAt: %s:%d\nMessage: %s", expr, file, line, message);
}

#  define CX_ASSERT(expr, message)                                                                 \
    ((expr) ? (void)0 : CX_ASSERT_failed(#expr, __FILE__, __LINE__, message))

#  define CX_WARNING(expr, message)                                                                \
    ((expr) ? (void)0 : WARNING_failed(#expr, __FILE__, __LINE__, message))

#  if defined(_MSC_VER) && !defined(_DEBUG)
#    undef CX_ASSERT
#    define CX_ASSERT(expr, message) ((void)0)
#  elif defined(NDEBUG)
#    undef CX_ASSERT
#    undef CX_ASSERT_1
#    undef CX_ASSERT_2
#    define CX_ASSERT_1(expr) ((void)0)
#    define CX_ASSERT_2(expr, message) ((void)0)
#    define GET_MACRO(_1, _2, NAME, ...) NAME
#    define CX_ASSERT(...) GET_MACRO(__VA_ARGS__, CX_ASSERT_2, CX_ASSERT_1)(__VA_ARGS__)
#  endif

#endif  //CXSTRUCTS_SRC_CXASSERT_H_
