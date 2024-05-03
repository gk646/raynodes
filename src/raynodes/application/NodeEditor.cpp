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

#include <ranges>
#include <cxstructs/Constraint.h>
#include <raygui.h>

#include "node/Node.h"
#include "shared/rayutils.h"

#include "application/elements/Action.h"
#include "application/editor/EditorControls.h"
#include "application/editor/EditorUI.h"
#include "application/editor/EditorExit.h"
#include "application/editor/EditorInit.h"
#include "application/editor/EditorUpdate.h"
#include "application/editor/EditorDraw.h"

NodeEditor::NodeEditor(const int argc, char* argv[]) : context(argc, argv) {
  Editor::SetupDisplay(context);
  Editor::SetupCamera(context);
}

bool NodeEditor::start() {
  cxstructs::Constraint<true> c;

  c + context.persist.loadWorkingDirectory(context);
  c + context.display.loadFont(context);
  c + context.display.loadIcons(context);
  c + context.plugin.loadPlugins(context);
  c + context.persist.loadFromFile(context);

  return c.holds();
}

namespace {
void DrawBackGround(EditorContext& ec) {
  Editor::DrawGrid(ec);
}
void DrawContent(EditorContext& ec) {
  Editor::DrawNodes(ec);

  const bool isCTRLDown = ec.input.isKeyDown(KEY_LEFT_CONTROL);
  Editor::DrawConnections(ec, isCTRLDown);

  if (ec.logic.isSelecting) {
    DrawRectangleRec(ec.logic.selectRect, ColorAlpha(isCTRLDown ? RED : BLUE, 0.4F));
  }

  if (ec.logic.isMakingConnection) {
    DrawLineBezier(ec.logic.draggedPinPos, ec.logic.worldMouse, 2, ec.logic.draggedPin->getColor());
  }
}
void DrawForeGround(EditorContext& ec) {
  Editor::UpdateTick(ec);  //Updates all nodes

  Editor::DrawContextMenu(ec);
  Editor::DrawActions(ec);
  Editor::DrawTopBar(ec);
  Editor::DrawStatusBar(ec);

  char buff[8];
  snprintf(buff, 8, "%d", GetFPS());
  DrawTextEx(ec.display.editorFont, buff, {25, 25}, 16, 1.0F, GREEN);

  Editor::PollControls(ec);
}
}  // namespace

int NodeEditor::run() {
  const auto& camera = context.display.camera;

  while (!context.logic.closeApplication && !WindowShouldClose()) {
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