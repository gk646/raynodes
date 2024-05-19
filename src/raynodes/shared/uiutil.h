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

#ifndef UIUTIL_H
#define UIUTIL_H

#include "shared/fwd.h"

#include <string>

#include <cxstructs/StackVector.h>
#include <cxutil/cxstring.h>
// Current limit of 150 -> stacked based so really doesnt matter
using SortVector = cxstructs::StackVector<const char*, 150>;

// Filters the strings based on prefix and then sorts after levenshtein distance
template <typename T>
void StringFilterMap(auto& map, const std::string& s, cxstructs::StackVector<T*, 150>& vec) {
  vec.clear();

  if constexpr (std::is_same_v<const char, T>) { volatile int s = 5; }

  const char* searchCStr = s.c_str();
  const int searchSize = s.size();

  for (auto& pair : map) {
    const auto* name = pair.first;  // Sort after keys
    if (searchSize == 0 || strncmp(name, searchCStr, searchSize) == 0) {
      if constexpr (std::is_same_v<NodeInfo, T>) {
        vec.push_back(&pair.second);
      } else if constexpr (std::is_same_v<const char, T>) {
        vec.push_back(pair.first);
      }
    }
  }

  size_t size = vec.size();
  if (size == 0) return;

  for (uint8_t i = 0; i < size - 1; ++i) {
    uint8_t minIndex = i;
    for (uint8_t j = i + 1; j < size; ++j) {
      const char* str1 = nullptr;
      const char* str2 = nullptr;
      if constexpr (std::is_same_v<NodeInfo, T>) {
        str1 = vec[j]->nTemplate.label;
        str2 = vec[minIndex]->nTemplate.label;
      } else if constexpr (std::is_same_v<const char, T>) {
        str1 = vec[j];
        str2 = vec[minIndex];
      }
      if (cxstructs::str_sort_levenshtein<PLG_MAX_NAME_LEN>(str1, searchCStr)
          < cxstructs::str_sort_levenshtein<PLG_MAX_NAME_LEN>(str2, searchCStr)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(vec[i], vec[minIndex]); }
  }
}

#endif  //UIUTIL_H