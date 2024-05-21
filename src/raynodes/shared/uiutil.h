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

// Case insensitive - filters based on exisiting substring
template <typename T, typename GetString>
void StringFilter(T* arg, const char* search, cxstructs::StackVector<T*, 150>& collector, GetString getString) {
  const char* name = getString(arg);
  if (cxstructs::str_substr_case(name, search) != nullptr) { collector.push_back(arg); }
}

// Sorts after case insensitive levenshtein
template <typename T, typename GetString>
void SortFilteredVector(cxstructs::StackVector<T*, 150>& vec, const char* search, GetString getString) {
  int size = vec.size();
  if (size == 0) return;
  for (uint8_t i = 0; i < size - 1; ++i) {
    uint8_t minIndex = i;
    for (uint8_t j = i + 1; j < size; ++j) {
      if (cxstructs::str_sort_levenshtein_case<PLG_MAX_NAME_LEN>(getString(vec[j]), search)
          < cxstructs::str_sort_levenshtein_case<PLG_MAX_NAME_LEN>(getString(vec[minIndex]), search)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(vec[i], vec[minIndex]); }
  }
}

template <typename T>
void StringFilterMap(auto& map, const std::string& s, cxstructs::StackVector<T*, 150>& vec) {
  vec.clear();
  const char* searchCStr = s.c_str();

  for (auto& pair : map) {
    if constexpr (std::is_same_v<NodeInfo, T>) {
      StringFilter(&pair.second, searchCStr, vec, [](T* arg) { return arg->nTemplate.label; });
    } else if constexpr (std::is_same_v<const char, T>) {
      StringFilter(pair.first, searchCStr, vec, [](T* arg) { return arg; });
    }
  }

  SortFilteredVector(vec, s.c_str(), [](T* arg) {
    if constexpr (std::is_same_v<NodeInfo, T>) {
      return arg->nTemplate.label;
    } else if constexpr (std::is_same_v<const char, T>) {
      return arg;
    }
  });
}

#endif  //UIUTIL_H