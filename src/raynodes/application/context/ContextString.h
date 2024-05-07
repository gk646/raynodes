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

#ifndef RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_
#define RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_

struct String {
  static constexpr int BUFFER_SIZE = 512;
  inline static char buffer[BUFFER_SIZE];

  const char* applicationDir;

  String();
  static const char* FormatText(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    return buffer;
  }
  static const char* GetPaddedNum(const int num) {
    char buff[6];
    snprintf(buff, 6, "%d", num);
    cxstructs::str_pad(buffer, 6, buff, '.');
    return buffer;
  }
  static void updateWindowTitle(EditorContext& ec);
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_