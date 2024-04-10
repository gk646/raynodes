#ifndef RAYNODES_SRC_PERSIST_PROPERTYSAVER_H_
#define RAYNODES_SRC_PERSIST_PROPERTYSAVER_H_

#include <cstdio>
#include <cxutil/cxstring.h>
#include <string>
#include <fstream>

// Writes a string property to the file
inline void addSaveProperty(FILE* file, const char* value) {
  fprintf(file, "%s|", value);
}

// Writes an integer or enum property to the file
inline void addSaveProperty(FILE* file, int value) {
  fprintf(file, "%d|", value);
}

// Writes a float property to the file
inline void addSaveProperty(FILE* file, float value) {
  fprintf(file, "%.*f|", 3, value);
}

inline void loadProperty(char** context, const char** s) {
  const auto res = cxstructs::str_parse_token<const char*>(context, '|');
  if (res) {
    *s = res;
  }
}

inline void loadProperty(char** context, std::string& s) {
  const auto res = cxstructs::str_parse_token<const char*>(context, '|');
  if (res) {
    s = res;
  }
}

inline void loadProperty(char** context, int& i) {
  i = cxstructs::str_parse_token<int>(context, '|');
}

inline void loadProperty(char** context, float& f) {
  f = cxstructs::str_parse_token<float>(context, '|');
}

#endif  //RAYNODES_SRC_PERSIST_PROPERTYSAVER_H_
