// Copyright (c) 2023 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define CX_FINISHED
#ifndef CXSTRUCTS_SRC_CXIO_H_
#  define CXSTRUCTS_SRC_CXIO_H_

#  include "../cxconfig.h"
#  include <cstring>

// Simple, readable and fast *symmetric* serialization structure with loading
// and saving. Each line is a concatenated list of values and a separator
// 13|3.145|This is a string|56|

// I didnt find a way around hard coding the separator...

// Using the CXX23 std::print() is about 10% slower

namespace cxstructs {
static constexpr int MAX_SECTION_SIZE = 16;
#  define NEW_LINE_SUB '\036'

//-----------SHARED-----------//
namespace {
// Dont wanna include <string>
int manual_strlen(const char* str) {
  int len = 0;
  while (str[len] != '\0')
    len++;
  return len;
}
int manual_strncmp(const char* s1, const char* s2, int n) {
  for (int i = 0; i < n && s1[i] && s2[i]; i++) {
    if (s1[i] != s2[i]) return s1[i] - s2[i];
  }
  return 0;
}
}  // namespace
// true if end of file // don't call this more than once per line
inline auto io_check_eof(FILE* file) -> bool {
  // Remember the current position
  long currentPos = ftell(file);
  if (currentPos == -1) return true;  // Error in ftell

  // Try to read a byte
  char ch;
  if (fread(&ch, 1, 1, file) != 1) {
    return true;  // EOF reached or read error
  }

  // Seek back to the original position
  fseek(file, currentPos, SEEK_SET);
  return false;  // Not EOF
}
//-----------SAVING-----------//
// Writes a section header - used like: while(io_load_inside_section()){}
inline void io_save_section(FILE* file, const char* value) {
  fprintf(file, "--%s--\n", value);
}
// Writes a new line to file
inline void io_save_newline(FILE* file) {
  fputc('\n', file);
}

// Writes a string value to file, replacing newlines with the SEPARATOR
inline void io_save(FILE* file, const char* value) {
  while (*value != '\0') {
    if (*value == '\n') {
      fputc(NEW_LINE_SUB, file);  // Replace newline with SEPARATOR
    } else {
      fputc(*value, file);  // Write the character as is
    }
    ++value;
  }
  fputc('\037', file);  // Add SEPARATOR at the end
}

// Writes an integer or enum property to the file
inline void io_save(FILE* file, const int value) {
  fprintf(file, "%d\037", value);
}

// Writes a boolean to the file
inline void io_save(FILE* file, const bool value) {
  fprintf(file, "%d\037", value ? 1 : 0);
}

// Writes a float to the file
inline void io_save(FILE* file, const float value) {
  fprintf(file, "%.3f\037", value);
}

// Writes three floats to the file - separated by ";"
inline void io_save(FILE* file, const float value, const float value2, const float value3) {
  fprintf(file, "%.6f;%.6f;%.6f\037", value, value2, value3);
}
// Writes three floats to the file - separated by ";"
inline void io_save(FILE* file, const float value, const float value2) {
  fprintf(file, "%.6f;%.6f\037", value, value2);
}

// Buffers the given SaveFunc to memory so the file is only written if it
// executes successfully. Returns false on error
template <typename SaveFunc>  // SaveFunc(FILE* file)
bool io_save_buffered_write(const char* fileName, const int memoryBufferBytes, SaveFunc func) {
  CX_ASSERT(memoryBufferBytes > 0, "Given buffer size invalid");

#  ifdef _WIN32
  FILE* file;
  fopen_s(&file, "NUL", "wb");
#  else
  FILE* file = fopen("/dev/null", "wb");
#  endif
  // Write to in memory buffer
  if (file == nullptr) { return false; }

  auto* buffer = new char[memoryBufferBytes];
  std::memset(buffer, 0, memoryBufferBytes);

  // _IOLBF is line buffering
  if (setvbuf(file, buffer, _IOFBF, memoryBufferBytes) != 0) {
    delete[] buffer;
    return false;
  }

  // Call the user function
  func(file);

  if (fclose(file) != 0) {
    delete[] buffer;
    return false;
  }

  // When successful, open the actual save file and save the data
  const int dataSize = (int)strlen(buffer);
#  ifdef _WIN32
  fopen_s(&file, fileName, "wb");
#  else
  file = fopen(fileName, "wb");
#  endif
  if (file == nullptr) {
    delete[] buffer;
    return false;
  }

  // Set no buffering for more efficiency
  if (setvbuf(file, nullptr, _IONBF, 0)) {
    delete[] buffer;
    return false;
  }

  fwrite(buffer, dataSize, 1, file);

  delete[] buffer;

  if (fclose(file) != 0) { return false; }

  return true;
}
//-----------LOADING-----------//
// Searches for the next new line but stops at the separator if not forced
inline void io_load_newline(FILE* file, bool force = false) {
  char ch;
  while (fread(&ch, 1, 1, file) == 1) {
    if (!force && ch == '\037') return;
    if (ch == '\n') return;
  }
}
inline void io_load_skip_separator(FILE* file) {
  char ch;
  while (fread(&ch, 1, 1, file) == 1) {
    if (ch == '\037') return;
  }
}
inline bool io_load_inside_section(FILE* file, const char* section) {
  const long currentPos = ftell(file);
  if (currentPos == -1) return false;  // Error - we return false

  char ch;
  if (fread(&ch, 1, 1, file) != 1) {
    fseek(file, currentPos, SEEK_SET);
    return false;
  }

  if (ch == '-') {
    if (fread(&ch, 1, 1, file) != 1 || ch != '-') {
      fseek(file, currentPos, SEEK_SET);
      return true;  // Not a new section marker
    }

    char buffer[MAX_SECTION_SIZE] = {};
    int count = 0;
    const int sectionLength = manual_strlen(section);
    while (fread(&ch, 1, 1, file) == 1 && ch != '-' && count < sectionLength && count < MAX_SECTION_SIZE - 1) {
      buffer[count++] = ch;
    }
    buffer[count] = '\0';  // Null-terminate the string
    if (manual_strncmp(buffer, section, sectionLength) == 0) {
      io_load_newline(file, false);
      return true;  // Found same section
    }
    io_load_newline(file, false);

    return false;  // Found new section
  }

  fseek(file, currentPos, SEEK_SET);
  return true;  // Still inside same section
}
// Returns true if the next byte is a newline - useful when loading a undefined amount in one line
inline bool io_load_is_newline(FILE* file) {
  char c;
  fread(&c, 1, 1, file);
  const auto result = c == '\n';
  fseek(file, -1, SEEK_CUR);
  return result;
}
// include <string> to use
#  if defined(_STRING_) || defined(_GLIBCXX_STRING)
inline void io_load(FILE* file, std::string& s) {
  //s.reserve(reserve_amount); // Dont need to reserve - string shouldnt allocate below 15 characters
  char ch;
  while (fread(&ch, 1, 1, file) == 1 && ch != '\037') {
    if (ch == NEW_LINE_SUB) [[unlikely]] { ch = '\n'; }
    s.push_back(ch);
  }
  while (ch != '\37' && fread(&ch, 1, 1, file) == 1) {}
}
#  endif
// Load a string property into a user-supplied buffer - return bytes written - reads until linesep is found
inline int io_load(FILE* file, char* buffer, size_t buffer_size) {
  int count = 0;
  char ch;
  while (count < buffer_size - 1 && fread(&ch, 1, 1, file) == 1 && ch != '\037') {
    if (ch == NEW_LINE_SUB) [[unlikely]] { ch = '\n'; }
    buffer[count++] = ch;
  }
  buffer[count] = '\0';
  while (ch != '\037' && fread(&ch, 1, 1, file) == 1) {}
  return count;
}
// Directly load an integer property from the file
inline void io_load(FILE* file, int& i) {
  fscanf(file, "%d\037", &i);
}

// Directly load a boolean from the file
inline void io_load(FILE* file, bool& value) {
  int num = 0;
  fscanf(file, "%d\037", &num);
  value = num == 1;
}

// Directly load a float from the file
inline void io_load(FILE* file, float& f) {
  fscanf(file, "%f\037", &f);
}

// Directly load three floats from the file
inline void io_load(FILE* file, float& f, float& f2, float& f3) {
  fscanf(file, "%f;%f;%f\037", &f, &f2, &f3);
}
// Directly load three floats from the file
inline void io_load(FILE* file, float& f, float& f2) {
  fscanf(file, "%f;%f\037", &f, &f2);
}
}  // namespace cxstructs

#endif  // CXSTRUCTS_SRC_CXIO_H_