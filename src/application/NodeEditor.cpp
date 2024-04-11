#include "NodeEditor.h"

#include <cxstructs/Constraint.h>

#include "application/editor/EditorInit.h"
#include "application/editor/EditorUI.h"
#include "application/editor/EditorUpdate.h"
#include "application/editor/EditorControls.h"
#include "application/editor/EditorDraw.h"
#include "application/editor/EditorExit.h"

NodeEditor::NodeEditor(const char* saveName) : context(saveName) {
  Editor::SetupDisplay(context);
  Editor::SetupCamera(context);
}

//TODO start logic thread
bool NodeEditor::start() {
  cxstructs::Constraint<true> c;

  c + context.display.loadFont();
  c + context.persist.loadFromFile(context);

  return true;
}

//TODO add custom frame control
int NodeEditor::run() {
  const auto& camera = context.display.camera;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground({90, 105, 136});
    {
      Editor::DrawBackGround(context);
      {
        BeginMode2D(camera);
        { Editor::DrawContent(context); }
        EndMode2D();
      }
      Editor::DrawForeGround(context);
    }
    EndDrawing();
  }


  return Editor::Exit(context);
}
