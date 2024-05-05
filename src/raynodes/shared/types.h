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

#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

struct Ints {
  uint16_t a;
  uint16_t b;
};

struct Pointer {
  void* data;
  uint32_t size;
};

// Dont wanna include raylib everywhere

struct Vec2 {
  float x;
  float y;
};

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Color4 {
  unsigned char r;  // Color red value
  unsigned char g;  // Color green value
  unsigned char b;  // Color blue value
  unsigned char a;  // Color alpha value
};

struct ComponentTemplate {
  const char* label = nullptr;
  const char* component = nullptr;
};

struct NodeTemplate {
  const char* label = nullptr;
  Color4 color = {0, 0, 0, 255};                      // BLACK
  ComponentTemplate components[COMPONENTS_PER_NODE];  // Current limit
};

#endif  //TYPES_H