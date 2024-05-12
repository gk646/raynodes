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
// and saving. Each line is a concatenated list of values and delimiter '|'
// (might be changeable in the future) 13|3.145|This is a string|56|

// Using the CXX23 std::print() is about 10% slower
namespace cxstructs {
// inline static constexpr char DELIMITER = '|'; //Not used yet
static constexpr int MAX_SECTION_SIZE = 16;

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
// Writes a string value to file
inline void io_save(FILE* file, const char* value) {
  fprintf(file, "%s|", value);
}
// Writes an integer or enum property to the file
inline void io_save(FILE* file, const int value) {
  fprintf(file, "%d|", value);
}
// Writes an boolean to the file
inline void io_save(FILE* file, const bool value) {
  fprintf(file, "%d|", value ? 1 : 0);
}
// Writes a float to the file
inline void io_save(FILE* file, const float value) {
  fprintf(file, "%.3f|", value);
}
// Writes a three floats to the file
inline void io_save(FILE* file, const float value, const float value2, const float value3) {
  fprintf(file, "%.3f|%.3f|%.3F|", value, value2, value3);
}
// Buffers the given SaveFunc to memory so the file is only written if it
// executes successfully. Returns false on error
template <typename SaveFunc>  // SaveFunc(FILE* file)
bool io_save_buffered_write(const char* fileName, const int memoryBufferBytes, SaveFunc func) {
  CX_ASSERT(memoryBufferBytes > 0, "Given buffer size invalid");

#  ifdef _WIN32
  FILE* file;
  fopen_s(&file,"NUL", "wb");
#  else
  FILE* file = fopen("/dev/null", "wb");
#  endif
  // Write to in memory buffer
  if (file == nullptr) {
    return false;
  }

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
  fopen_s(&file,fileName, "wb");
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

  if (fclose(file) != 0) {
    return false;
  }

  return true;
}
//-----------LOADING-----------//
// Searches for the next new line but stops at the delimiter if not forced
inline void io_load_newline(FILE* file, bool force = false) {
  char ch;
  while (fread(&ch, 1, 1, file) == 1) {
    if (!force && ch == '|') return;
    if (ch == '\n') return;
  }
}
inline bool io_load_inside_section(FILE* file, const char* section) {
  long currentPos = ftell(file);
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
    int sectionLength = manual_strlen(section);
    while (fread(&ch, 1, 1, file) == 1 && count < sectionLength && count < MAX_SECTION_SIZE - 1) {
      buffer[count++] = ch;
    }
    buffer[count] = '\0';  // Null-terminate the string
    if (manual_strncmp(buffer, section, sectionLength) == 0) {
      io_load_newline(file, false);
      return true;  // Found same section
    } else {
      io_load_newline(file, false);
      return false;  // Found new section
    }
  }

  fseek(file, currentPos, SEEK_SET);
  return true;  // Still inside same section
}
// include <string> to use
#  if defined( _STRING_) || defined(_GLIBCXX_STRING)
inline void io_load(FILE* file, std::string& s, int reserve_amount = 15) {
  s.reserve(reserve_amount);
  char ch;
  while (fread(&ch, 1, 1, file) == 1 && ch != '|') {
    s.push_back(ch);
  }
}
#  endif
// Load a string property into a user-supplied buffer
inline void io_load(FILE* file, char* buffer, size_t buffer_size) {
  size_t count = 0;
  char ch;
  while (count < buffer_size - 1 && fread(&ch, 1, 1, file) == 1 && ch != '|') {
    buffer[count++] = ch;
  }
  buffer[count] = '\0';
}
// Directly load an integer property from the file
inline void io_load(FILE* file, int& i) {
  fscanf(file, "%d|", &i);
}
// Directly load a boolean to the file
inline void io_load(FILE* file, bool& value) {
  int num = 0;
  fscanf(file, "%d|", &num);
  value = num == 1;
}
// Directly load a float from the file
inline void io_load(FILE* file, float& f) {
  fscanf(file, "%f|", &f);
}
// Directly load a three floats from the file
inline void io_load(FILE* file, float& f, float& f2, float& f3) {
  fscanf(file, "%f|%f|%f|", &f, &f2, &f3);
}

}  // namespace cxstructs

#  ifdef CX_INCLUDE_TESTS
#    include <chrono>
namespace cxtests {
using namespace cxstructs;
using namespace std::chrono;
static void benchMark() {
  FILE* file;
  const char* filename = "hello.txt";
  constexpr int num = 1000;
  int val = 5;
  auto start_write = high_resolution_clock::now();
  fopen_s(&file, filename, "wb");
  if (file != nullptr) {
    for (int i = 0; i < num; i++) {
      for (int j = 0; j < num; j++) {
        io_save(file, j);
      }
      io_save_newline(file);
    }
    fclose(file);
  }
  auto end_write = high_resolution_clock::now();
  auto start_read = high_resolution_clock::now();

  fopen_s(&file, filename, "rb");
  if (file != nullptr) {
    for (int i = 0; i < num; i++) {
      for (int j = 0; j < num; j++) {
        io_load(file, val);
      }
    }
    fclose(file);
  }
  auto end_read = high_resolution_clock::now();
  auto duration_write = duration_cast<milliseconds>(end_write - start_write).count();
  auto duration_read = duration_cast<milliseconds>(end_read - start_read).count();
  printf("Write time: %lld ms\n", duration_write);
  printf("Read time: %lld ms\n", duration_read);
}
void test_save_load_string() {
  const char* test_filename = "test_string.txt";
  const char* original_string = "Hello, world!";
  char buffer[256];

  // Save
  FILE* file = std::fopen(test_filename, "wb");
  cxstructs::io_save(file, original_string);
  cxstructs::io_save_newline(file);
  std::fclose(file);

  // Load
  file = std::fopen(test_filename, "rb");
  cxstructs::io_load(file, buffer, sizeof(buffer));
  std::fclose(file);

  // Assert
  CX_ASSERT(std::strcmp(original_string, buffer) == 0, "String save/load failed");
}
void test_save_load_int() {
  const char* test_filename = "test_int.txt";
  const int original_int = 42;
  int loaded_int;

  // Save
  FILE* file = std::fopen(test_filename, "wb");
  cxstructs::io_save(file, original_int);
  cxstructs::io_save_newline(file);
  std::fclose(file);

  // Load
  file = std::fopen(test_filename, "rb");
  cxstructs::io_load(file, loaded_int);
  std::fclose(file);

  // Assert
  CX_ASSERT(original_int == loaded_int, "Int save/load failed");
}
void test_save_load_float() {
  const char* test_filename = "test_float.txt";
  constexpr float original_float = 3.141;
  float loaded_float;

  // Save
  FILE* file;
  fopen_s(&file, test_filename, "wb");
  cxstructs::io_save(file, original_float);
  cxstructs::io_save_newline(file);
  std::fclose(file);

  // Load
  fopen_s(&file, test_filename, "rb");
  cxstructs::io_load(file, loaded_float);
  std::fclose(file);

  // Assert
  CX_ASSERT(original_float == loaded_float, "Float save/load failed");
}
void delete_test_files() {
  // List of test files to delete
  const char* files[] = {"test_string.txt", "test_int.txt", "test_float.txt", "test_complex.txt", "hello.txt"};

  // Iterate over the array and delete each file
  for (const char* filename : files) {
    if (std::remove(filename) != 0) {
      perror("Error deleting file");
    } else {
      printf("%s deleted successfully.\n", filename);
    }
  }
}
void test_complex_save_load() {
  const char* test_filename = "test_complex.txt";
  const char* original_str1 = "TestString1";
  const int original_int = 123;
  const float original_float = 456.789f;
  const char* original_str2 = "TestString2";

  char buffer_str1[256];
  int loaded_int = -1;
  float loaded_float = -1;
  std::string buffer_str2;
  std::string hello;
  std::string bye;
  std::string hello2;

  // Save complex data
  FILE* file;
  fopen_s(&file, test_filename, "wb");
  if (file) {
    cxstructs::io_save(file, original_str1);
    cxstructs::io_save(file, original_int);
    cxstructs::io_save(file, original_float);
    cxstructs::io_save(file, original_str2);
    cxstructs::io_save_newline(file);
    io_save(file, "hello");
    io_save(file, "bye");
    io_save(file, "hello");
    std::fclose(file);
  }

  // Load complex data
  file = std::fopen(test_filename, "rb");
  if (file) {
    cxstructs::io_load(file, buffer_str1, sizeof(buffer_str1));
    cxstructs::io_load(file, loaded_int);
    cxstructs::io_load(file, loaded_float);
    cxstructs::io_load(file, buffer_str2);
    io_load_newline(file);
    io_load(file, hello);
    io_load(file, bye);
    io_load(file, hello2);
    std::fclose(file);
  }

  // Assert all loaded data matches original
  CX_ASSERT(std::strcmp(original_str1, buffer_str1) == 0, "String1 save/load failed");
  CX_ASSERT(original_int == loaded_int, "Int save/load failed");
  CX_ASSERT(std::fabs(original_float - loaded_float) < 0.001,
            "Float save/load failed");  // Allow for slight floating-point inaccuracies
  CX_ASSERT(std::strcmp(original_str2, buffer_str2.c_str()) == 0, "String2 save/load failed");
  CX_ASSERT(hello == hello2, "");
  CX_ASSERT(bye == "bye", "");
}
static void TEST_IO() {
  benchMark();
  test_save_load_float();
  test_save_load_int();
  test_save_load_string();
  test_complex_save_load();
  delete_test_files();
}
}  // namespace cxtests
#  endif
#endif  // CXSTRUCTS_SRC_CXIO_H_