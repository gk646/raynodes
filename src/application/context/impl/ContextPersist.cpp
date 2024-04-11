#include "application/context/ContextPersist.h"

#include <cxutil/cxio.h>

#include "application/EditorContext.h"
#include "nodes/Nodes.h"

bool Persist::saveToFile(EditorContext& ec) const {
  const auto& nodes = ec.core.nodes;
  if (nodes.empty()) return true;  //Nothing to save

  FILE* file;
  int count = 0;

  //Write to in memory buffer
  if (fopen_s(&file, "NUL", "w") != 0) return false;

  int buffSize = (int)nodes.size() * 1000;
  char* buffer = new char[buffSize];
  std::memset(buffer, 0, buffSize);

  if (setvbuf(file, buffer, _IOLBF, buffSize) != 0) {
    delete[] buffer;
    return false;
  }

  //Save nodes
  for (auto n : nodes) {
    n->saveState(file);
    cxstructs::io_save_newline(file);
    count++;
  }

  if (fclose(file) != 0) return false;

  //When successful, open the actual save file and save the data
  int dataSize = (int)strlen(buffer);
  if (fopen_s(&file, openedFile, "w") != 0) {
    delete[] buffer;
    return false;
  }

  fwrite(buffer, dataSize, 1, file);
  
  delete[] buffer;
  if (fclose(file) != 0) return false;
  printf("Saved %d nodes.\n", count);
  return true;
}

bool Persist::loadFromFile(EditorContext& ec) const {
  FILE* file;

  if (fopen_s(&file, openedFile, "r") != 0) return false;

  if (!file) {
    fprintf(stderr, "Unable to open file %s\n", openedFile);
    return true;
  }

  int count = 0;
  int type = -1;
  while (!cxstructs::io_check_eof(file)) {
    cxstructs::io_load(file, type);
    auto newNode = ec.core.createNode(NodeType(type), {0, 0});
    if (!newNode) {
      cxstructs::io_load_newline(file, true);
      continue;
    }
    newNode->loadState(file);
    cxstructs::io_load_newline(file);
    count++;
  }

  printf("Loaded %d nodes.\n", count);
  if (fclose(file) != 0) return false;
  return true;
}
