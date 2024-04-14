#include "application/NodeEditor.h"


#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main(int argc, char* argv[]) {
  NodeEditor editor{"saves/FirstQuest.rn"};
  if (editor.start()) {
    return editor.run();
  }
}