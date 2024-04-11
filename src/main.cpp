#include "application/NodeEditor.h"

int main(int argc, char* argv[]) {
  auto* editor = new NodeEditor("saves/FirstQuest.rn");
  if (editor->start()) {
    return editor->run();
  }
}
