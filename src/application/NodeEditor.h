#ifndef RAYNODES_SRC_NODEEDITOR_H_
#define RAYNODES_SRC_NODEEDITOR_H_

#include <thread>

#include "EditorContext.h"
class NodeEditor {
  EditorContext context;
  std::thread updateThread;

  void update();

 public:
  explicit NodeEditor(const char* saveName);
  ~NodeEditor();
  bool start();
  int run();
};
#endif  //RAYNODES_SRC_NODEEDITOR_H_
