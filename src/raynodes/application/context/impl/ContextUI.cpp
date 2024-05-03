#include "application/EditorContext.h"

void UserInterface::invokeFileMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 5) ec.logic.closeApplication = true;
}
void UserInterface::invokeEditMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;
}
void UserInterface::invokeViewMenu(EditorContext& ec, int i) {
  if (i == -1 || i == 0) return;

  if (i == 1) ec.display.zoomIn();
  if (i == 2) ec.display.zoomOut();
  if (i == 4) ec.ui.showGrid = !ec.ui.showGrid;
}