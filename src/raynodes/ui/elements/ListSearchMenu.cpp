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

#include <ranges>
#include <unordered_map>

#include <cxstructs/StackVector.h>
#include <cxutil/cxstring.h>
#include "ListSearchMenu.h"

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
      if (cxstructs::str_sort_levenshtein_prefix<PLG_MAX_NAME_LEN>(sortVec[j], searchCStr)
          < cxstructs::str_sort_levenshtein_prefix<PLG_MAX_NAME_LEN>(sortVec[minIndex], searchCStr)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(sortVec[i], sortVec[minIndex]); }
  }
}
}  // namespace

SortVector ListSearchMenu::GetSortedVector(const auto& mapKeys, const std::string& search) {
  SortVector vector;
  stringSort(mapKeys, search, vector);
  return vector;
}

template SortVector ListSearchMenu::GetSortedVector(
    const std::unordered_map<const char*, ComponentCreateFunc, cxstructs::Fnv1aHash, cxstructs::StrEqual>& map,
    const std::string&);