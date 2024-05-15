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

#include <raylib.h>
#include <ranges>

#include <cxstructs/StackVector.h>
#include <cxutil/cxstring.h>
#include "ListSearchMenu.h"

#include "application/EditorContext.h"
#include "ui/elements/TextField.h"

// Current limit of 150 -> stacked based so really doesnt matter
using SortVector = cxstructs::StackVector<const char*, 150, uint16_t>;

namespace {
void stringSort(const auto& map, const std::string& searchText, SortVector& sortVec) {
  sortVec.clear();

  for (auto& value : map | std::ranges::views::keys) {
    sortVec.push_back(value);
  }

  const char* searchCStr = searchText.c_str();
  size_t size = sortVec.size();

  for (size_t i = 0; i < size - 1; ++i) {
    size_t minIndex = i;
    for (size_t j = i + 1; j < size; ++j) {
      if (cxstructs::str_sort_levenshtein_prefix<Plugin::MAX_NAME_LEN>(sortVec[j], searchCStr)
          < cxstructs::str_sort_levenshtein_prefix<Plugin::MAX_NAME_LEN>(sortVec[minIndex], searchCStr)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(sortVec[i], sortVec[minIndex]); }
  }
}
}  // namespace

template <typename V, typename Hash, typename Comp>
const char* ListSearchMenu::Draw(EditorContext& ec, Vector2 pos, TextField& searchBar,
                                 const std::unordered_map<const char*, V, Hash, Comp>& map) {
  // Cache
  constexpr float entryWidth = 150.0f;
  constexpr float padding = 4;
  const auto& font = ec.display.editorFont;
  const float fs = ec.display.fontSize;
  const auto entryHeight = fs + padding;
  const auto mouse = ec.logic.mouse;
  const int entries = map.size() + 1;

  // Draw BackGround
  auto bounds = ec.display.getFullyScaled({pos.x, pos.y, entryWidth, entries * entryHeight});
  DrawRectangleRec(bounds, UI::COLORS[UI_DARK]);

  if (!CheckCollisionPointRec(mouse, bounds)) return UI::DUMMY_STRING;  // return code for close

  // Draw SearchBar
  searchBar.font = &ec.display.editorFont;
  searchBar.bounds = bounds;
  searchBar.bounds.height = fs;

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) searchBar.onFocusGain(mouse);
  searchBar.update(ec);
  searchBar.draw();
  bounds.y += entryHeight;

  // Sort
  SortVector sortedStrings;
  stringSort(map, searchBar.buffer, sortedStrings);

  const char* selected = nullptr;
  for (const auto str : sortedStrings) {
    const auto currentRect = Rectangle{bounds.x, bounds.y, entryWidth, entryHeight};
    if (CheckCollisionPointRec(mouse, currentRect)) {
      DrawRectangleRounded(currentRect, 0.1F, 30, UI::COLORS[UI_MEDIUM]);
      if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) { selected = str; }
    } else {
      DrawRectangleRec(currentRect, UI::COLORS[UI_DARK]);
    }

    constexpr auto text = UI::COLORS[UI_LIGHT];
    DrawTextEx(font, str, {bounds.x + padding, bounds.y + padding / 2.0F}, fs, 0.0F, text);
    bounds.y += entryHeight;
  }

  return selected;
}
template const char* ListSearchMenu::Draw(
    EditorContext& ec, Vector2 pos, TextField& searchBar,
    const std::unordered_map<const char*, ComponentCreateFunc, cxstructs::Fnv1aHash, cxstructs::StrEqual>& map);