#ifndef RAYNODES_SRC_GRAPHICS_DROPDOWN_H_
#define RAYNODES_SRC_GRAPHICS_DROPDOWN_H_

#include "shared/fwd.h"

#include <string>
#include <vector>

struct DropDown {
  float x, y, w, h;  // no <raylib> compatibility
  bool isExpanded = false;
  int selectedIndex = 0;
  std::vector<std::string> items;

  void draw(EditorContext& ec, float x, float y);
  int update(EditorContext& ec);
};

#endif  //RAYNODES_SRC_GRAPHICS_DROPDOWN_H_