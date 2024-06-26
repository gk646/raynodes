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

#ifndef RAYNODES_SRC_GRAPHICS_DROPDOWN_H_
#define RAYNODES_SRC_GRAPHICS_DROPDOWN_H_

#include "shared/uiutil.h"

#include <vector>
#include <string>

#include <raylib.h>

// This class is need as raygui controls dont work in worldspace -> raygui functions still use screen space mouse

struct EXPORT SimpleDropDown final {
  std::vector<std::string> items;
  float x, y, w, h;  // no <raylib> compatibility
  int selectedIndex = 0;
  int hoveredIndex = 0;
  bool isExpanded = false;

  void draw(EditorContext& ec, float x, float y);
  int update(EditorContext& ec);

  static void DrawSearchDropdown(EditorContext& ec, Vector2 pos, TextField& search, const SortVector& items);
};

#endif  //RAYNODES_SRC_GRAPHICS_DROPDOWN_H_