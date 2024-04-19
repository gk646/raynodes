#include "application/NodeEditor.h"


int main(int argc, char* argv[]) {
  NodeEditor editor{"saves/FirstQuest.rn"};
  if (editor.start()) {
    return editor.run();
  }
  return 1;
}