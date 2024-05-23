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
#ifndef CXSTRUCTS_SRC_CXUTIL_CXSTRING_H_
#  define CXSTRUCTS_SRC_CXUTIL_CXSTRING_H_

#  include <cstdint>
#  include <cstdio>
#  include <cstring>
#  include <cctype>

namespace cxstructs {
// Pads the given string "arg" inside "buff" with the "padSymbol" - optional prefix and suffix
inline void str_pad(char* buff, const int size, const char* arg, const char padSymbol, const char* prefix = nullptr,
                    const char* suffix = nullptr) {
  int currPos = 0;
  std::memset(buff, 0, size);

  if (prefix) {
    currPos += snprintf(buff, size, "%s", prefix);
    currPos = currPos > size ? size : currPos;
  }
  if (currPos < size) {
    currPos += snprintf(buff + currPos, size - currPos, "%s", arg);
    currPos = currPos > size ? size : currPos;
  }
  if (suffix && currPos < size) { snprintf(buff + currPos, size - currPos, "%s", suffix); }

  for (int i = currPos; i < size - 1; i++) {
    if (buff[i] == '\0') { buff[i] = padSymbol; }
  }
  buff[size - 1] = '\0';
}
// Measure the length of the given string "arg" using a while loop
constexpr int str_len(const char* arg) {
  int len = 0;
  while (*arg) {
    arg++;
    len++;
  }
  return len;
}
// Returns an allocated copy of the given string "arg" (with new[]);
[[nodiscard("Allocates new string")]] inline char* str_dup(const char* arg) {
  const int len = str_len(arg);
  auto* newBuff = new char[len + 1];
  std::memcpy(newBuff, arg, len+1);
  return newBuff;
}
// Compares to string with a while loop
inline bool str_cmp(const char* arg, const char* arg2) {
  while (*arg && *arg2) {
    if (*arg != *arg2) return false;
    arg++;
    arg2++;
  }
  return *arg == *arg2;  // Both should be '\0' if strings are truly equal
}
// Case insensitive! - Compares to string with a while loop - stops at max count
inline int str_cmpn_case(const char* s1, const char* s2, int maxCount) {
  while (*s1 && *s2 && maxCount > 0) {
    const int diff = tolower(*s1) - tolower(*s2);
    if (diff != 0) return diff;
    ++s1;
    ++s2;
    maxCount--;
  }
  return tolower(*s1) - tolower(*s2);
}
// Case insensitive! - Tries to find and return the first occurrence of sequence in string
inline const char* str_substr_case(const char* string, const char* sequence) {
  if (!*sequence) return string;

  for (const char* p = string; *p; ++p) {
    if (tolower(*p) == tolower(*sequence)) {
      const char *h = p, *n = sequence;
      while (*h && *n && tolower(*h) == tolower(*n)) {
        ++h;
        ++n;
      }
      if (!*n) return p;
    }
  }
  return nullptr;
}
// Parses the given string into a int on best effort basis - stops when encountering any non numeric characters
inline int str_parse_int(const char* str, const int radix = 10) {
  if (str == nullptr || *str == '\0') return 0;

  int result = 0;
  bool negative = false;
  if (*str == '-') {
    negative = true;
    ++str;
  }

  while (*str) {
    char digit = *str;
    int value;
    if (digit >= '0' && digit <= '9') value = digit - '0';
    else if (digit >= 'a' && digit <= 'z') value = 10 + digit - 'a';
    else if (digit >= 'A' && digit <= 'Z') value = 10 + digit - 'A';
    else break;

    if (value >= radix) break;

    result = result * radix + value;
    ++str;
  }

  return negative ? -result : result;
}
// Parses the given string into a int64 on best effort basis
inline int64_t str_parse_long(const char* str, const int radix = 10) {
  if (str == nullptr || *str == '\0') return 0;

  int64_t result = 0;
  bool negative = false;
  if (*str == '-') {
    negative = true;
    ++str;
  }

  while (*str) {
    char digit = *str;
    int64_t value;
    if (digit >= '0' && digit <= '9') value = digit - '0';
    else if (digit >= 'a' && digit <= 'z') value = 10 + digit - 'a';
    else if (digit >= 'A' && digit <= 'Z') value = 10 + digit - 'A';
    else break;

    if (value >= radix) break;

    result = result * radix + value;
    ++str;
  }

  return negative ? -result : result;
}
// Parses the given string into a float on best effort basis
inline float str_parse_float(const char* str) {
  if (str == nullptr || *str == '\0') return 0.0F;

  float result = 0.0;
  bool negative = false;
  if (*str == '-') {
    negative = true;
    ++str;
  }

  // Parse the integer part
  while (*str && *str != '.') {
    if (*str < '0' || *str > '9') break;
    result = result * 10 + (*str - '0');
    ++str;
  }

  // Parse the fractional part
  if (*str == '.') {
    ++str;
    float factor = 0.1;
    while (*str && *str >= '0' && *str <= '9') {
      result += (*str - '0') * factor;
      factor *= 0.1;
      ++str;
    }
  }

  return negative ? -result : result;
}
// Parses the given string into a double on best effort basis
inline double str_parse_double(const char* str) {
  if (str == nullptr || *str == '\0') return 0.0;

  double result = 0.0;
  bool negative = false;
  if (*str == '-') {
    negative = true;
    ++str;
  }

  // Parse the integer part
  while (*str && *str != '.') {
    if (*str < '0' || *str > '9') break;
    result = result * 10 + (*str - '0');
    ++str;
  }

  // Parse the fractional part
  if (*str == '.') {
    ++str;
    double factor = 0.1;
    while (*str && *str >= '0' && *str <= '9') {
      result += (*str - '0') * factor;
      factor *= 0.1;
      ++str;
    }
  }

  return negative ? -result : result;
}
// Returns the Levenshtein distance for the two given strings
// This penalizes differences in length
// Failure: returns -1
template <int MAX_LEN>
int str_sort_levenshtein(const char* s1, const char* s2) {
  if (s1 == nullptr || s2 == nullptr) {
    return -1;  // Error: string length exceeds maximum allowed length
  }

  const size_t len1 = strlen(s1);
  const size_t len2 = strlen(s2);

  if (len1 > MAX_LEN || len2 > MAX_LEN) {
    return -1;  // Error: string length exceeds maximum allowed length
  }

  unsigned int d[MAX_LEN + 1][MAX_LEN + 1] = {0};

  for (unsigned int i = 0; i <= len1; ++i)
    d[i][0] = i;
  for (unsigned int j = 0; j <= len2; ++j)
    d[0][j] = j;

  for (unsigned int i = 1; i <= len1; ++i) {
    for (unsigned int j = 1; j <= len2; ++j) {
      unsigned int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
      unsigned int above = d[i - 1][j] + 1;
      unsigned int left = d[i][j - 1] + 1;
      unsigned int diag = d[i - 1][j - 1] + cost;

      d[i][j] = above < left ? above : left < diag ? left : diag;
    }
  }

  return d[len1][len2];
}
// Case insensitive! - Levenshtein distance
template <int MAX_LEN>
int str_sort_levenshtein_case(const char* s1, const char* s2) {
  const size_t len1 = strlen(s1);
  const size_t len2 = strlen(s2);

  if (len1 > MAX_LEN || len2 > MAX_LEN) {
    return -1;  // Error: string length exceeds maximum allowed length
  }

  unsigned int d[MAX_LEN + 1][MAX_LEN + 1] = {0};

  for (unsigned int i = 0; i <= len1; ++i)
    d[i][0] = i;
  for (unsigned int j = 0; j <= len2; ++j)
    d[0][j] = j;

  for (unsigned int i = 1; i <= len1; ++i) {
    for (unsigned int j = 1; j <= len2; ++j) {
      unsigned int cost = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1;
      unsigned int above = d[i - 1][j] + 1;
      unsigned int left = d[i][j - 1] + 1;
      unsigned int diag = d[i - 1][j - 1] + cost;

      d[i][j] = above < left ? above : left < diag ? left : diag;
    }
  }

  return d[len1][len2];
}
// string hash function
constexpr auto str_hash_fnv1a_32(char const* s) noexcept -> uint32_t {
  uint32_t hash = 2166136261U;
  while (*s != 0) {
    hash ^= (uint32_t)*s++;
    hash *= 16777619U;
  }
  return hash;
}
// Hash operator for a const char* for e.g. std::unordered_map
struct Fnv1aHash {
  auto operator()(const char* s) const noexcept -> std::size_t {
    uint32_t hash = 2166136261U;
    while (*s != 0) {
      hash ^= (uint32_t)(*s++);
      hash *= 16777619U;
    }
    return hash;
  }
};
// Equal operator for a const char* for e.g. std::unordered_map
struct StrEqual {
  bool operator()(const char* s1, const char* s2) const { return std::strcmp(s1, s2) == 0; }
};
#  if defined(_STRING_) || defined(_GLIBCXX_STRING)
// Replaces the string with the string representation of the given number
inline void str_embed_num(std::string& s, const float num) {
  s.clear();
  char buff[10];
  snprintf(buff, sizeof(buff), "%.6f", num);
  s.append(buff);
}
inline void str_embed_num(std::string& s, const double num) {
  s.clear();
  char buff[10];
  snprintf(buff, sizeof(buff), "%.6f", num);
  s.append(buff);
}
inline void str_embed_num(std::string& s, const int64_t num) {
  s.clear();
  char buff[15];
  snprintf(buff, sizeof(buff), "%lld", num);
  s.append(buff);
}

#  endif
}  // namespace cxstructs

#endif  //CXSTRUCTS_SRC_CXUTIL_CXSTRING_H_