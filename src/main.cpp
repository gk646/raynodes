
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "editor/NodeEditor.h"


int main() {
  auto* editor = new NodeEditor();
  if (editor->start()) {
    return editor->run();
  }
}

