#include "Action.h"

void TextAction::undo() {
  targetText = beforeState;
}

void TextAction::redo() {
  targetText = afterState;
}

