#include "NodeEditor.h"

#include <cxstructs/Constraint.h>

#include "node/Node.h"

#include "application/elements/Action.h"
#include "application/editor/EditorControls.h"
#include "application/editor/EditorUI.h"
#include "application/editor/EditorExit.h"
#include "application/editor/EditorInit.h"
#include "application/editor/EditorUpdate.h"
#include "application/editor/EditorDraw.h"

NodeEditor::NodeEditor(const char* saveName) : context(saveName) {
  Editor::SetupDisplay(context);
  Editor::SetupCamera(context);
}

NodeEditor::~NodeEditor() {
  if (updateThread.joinable()) {
    updateThread.join();
  }
}

bool NodeEditor::start() {
  cxstructs::Constraint<true> c;

  c + context.display.loadFont();
  c + context.persist.loadFromFile(context);

  //updateThread = std::thread(&NodeEditor::update, this);

  return c.holds();
}

namespace {
void DrawBackGround(EditorContext& ec) {
  Editor::DrawGrid(ec);
}
void DrawContent(EditorContext& ec) {
  auto& connections = ec.core.connections;

  Editor::DrawNodes(ec);

  for (const auto con : connections) {
    DrawLineBezier(con->getFromPos(), con->getToPos(), 2, con->out.getColor());
  }

  if (ec.logic.isSelecting) {
    DrawRectanglePro(ec.logic.selectRect, {0, 0}, 0, ColorAlpha(BLUE, 0.4F));
  }

  if (ec.logic.isMakingConnection) {
    DrawLineBezier(ec.logic.draggedPinPos, ec.logic.worldMouse, 2, ec.logic.draggedPin->getColor());
  }
}
void DrawForeGround(EditorContext& ec) {
  Editor::UpdateTick(ec);

  if (ec.logic.showContextMenu) {
    Editor::DrawContextMenu(ec);
  }

  Editor::DrawActions(ec);

  Editor::PollControls(ec);

  char buff[8];
  snprintf(buff, 8, "%d", GetFPS());
  DrawTextEx(ec.display.editorFont, buff, {25, 25}, 16, 1.0F, GREEN);
}
}  // namespace

int NodeEditor::run() {
  const auto& camera = context.display.camera;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground({90, 105, 136});
    {
      DrawBackGround(context);
      {
        BeginMode2D(camera);
        { DrawContent(context); }
        EndMode2D();
      }
      DrawForeGround(context);
    }
    EndDrawing();
  }

  return Editor::Exit(context);
}