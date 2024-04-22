// Copyright (c) 2024 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "NodeEditor.h"

#include <cxstructs/Constraint.h>

#include "node/Node.h"
#include "shared/rayutils.h"

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

bool NodeEditor::start() {
  cxstructs::Constraint<true> c;

  c + context.display.loadFont(context);
  c + context.persist.loadFromFile(context);

  return c.holds();
}

namespace {
void DrawBackGround(EditorContext& ec) {
  Editor::DrawGrid(ec);
}
void DrawContent(EditorContext& ec) {
  const auto& connections = ec.core.connections;

  Editor::DrawNodes(ec);

  bool isCTRLDown = ec.input.isKeyDown(KEY_LEFT_CONTROL);
  bool delNodes = isCTRLDown && ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT);
  const auto selectRect = ec.logic.selectRect;

  ConnectionDeleteAction* action = nullptr;
  if (delNodes) action = new ConnectionDeleteAction(10);
  for (const auto conn : connections) {
    const auto fromPos = conn->getFromPos();
    const auto toPos = conn->getToPos();
    DrawLineBezier(fromPos, toPos, 2, conn->out.getColor());

    if (delNodes && CheckCollisionBezierRect(fromPos, toPos, selectRect)) {
      action->deletedConnections.push_back(conn);
      ec.core.removeConnection(conn);
    }
  }
  if (delNodes) ec.core.addEditorAction(action);

  if (ec.logic.isSelecting) {
    DrawRectanglePro(selectRect, {0, 0}, 0, ColorAlpha(isCTRLDown ? RED : BLUE, 0.4F));
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