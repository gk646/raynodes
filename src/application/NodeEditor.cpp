#include "NodeEditor.h"

#include <cxstructs/Constraint.h>


#include "application/editor/EditorInit.h"
#include "application/editor/EditorUI.h"
#include "application/editor/EditorControls.h"
#include "application/editor/EditorUpdate.h"
#include "application/editor/EditorDraw.h"
#include "application/editor/EditorExit.h"

NodeEditor::NodeEditor(const char* saveName) : context(saveName) {
  Editor::SetupDisplay(context);
  Editor::SetupCamera(context);
}

NodeEditor::~NodeEditor() {
  if (updateThread.joinable()) {
    updateThread.join();
  }
}

//TODO start logic thread
bool NodeEditor::start() {
  cxstructs::Constraint<true> c;

  c + context.display.loadFont();
  c + context.persist.loadFromFile(context);

  updateThread = std::thread(&NodeEditor::update, this);

  return c.holds();
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

void NodeEditor::update() {
  using namespace std::chrono;
  using namespace std::this_thread;
  constexpr auto tickDuration = microseconds(1'000'000 / Core::targetLogicTicks);
  auto lastTime = steady_clock::now();
  microseconds accumulator(0);
  while (context.core.logicThreadRunning) {
    auto now = steady_clock::now();
    auto passedTime = duration_cast<microseconds>(now - lastTime);
    lastTime = now;
    accumulator += passedTime;
    if (accumulator >= tickDuration) {
      const auto startTime = steady_clock::now();

      //Logic Tick
      { Editor::UpdateTick(context); }

      const auto tickTime = steady_clock::now() - startTime;
      context.core.logicTickTime = (int)tickTime.count();
      accumulator = milliseconds(0);
    }

    sleep_for(microseconds(1));
  }
}
