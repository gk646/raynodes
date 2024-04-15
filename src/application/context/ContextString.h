#ifndef RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_
#define RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_

struct String {
  static constexpr int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  const char* formatText(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    return buffer;
  }
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXTSTRING_H_