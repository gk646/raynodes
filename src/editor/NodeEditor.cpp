#include "NodeEditor.h"

#include <graphics/ComponentRenderer.h>
#include <nodes/Nodes.h>
#include "EditorContext.h"

NodeEditor::NodeEditor() {
  if (!context) context = new EditorContext();
  const int screenWidth = 1280;
  const int screenHeight = 960;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raynodes");
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  auto& camera = context->camera;
  camera.target = {0.0f, 0.0f};
  camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

bool NodeEditor::start() {
  context->editorFont = LoadFont("res/monogram.ttf");
  return true;
}
int NodeEditor::run() {
  auto& camera = context->camera;
  onStart();
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground({90, 105, 136});
    {
      //Back
      drawBackGround();
      {
        BeginMode2D(camera);
        {
          //Content
          drawContent();

          //Draw user Content
          onFrame();
        }
        EndMode2D();
      }
      drawForeGround();
    }
    EndDrawing();
  }
  onEnd();
  CloseWindow();
  return 0;
}

void NodeEditor::drawBackGround() {
  auto& camera = context->camera;
  cr::DrawGrid(camera, 20.0F);
}

void NodeEditor::drawContent() {
  auto& nodes = context->nodes;

  auto mouse = GetMousePosition();
  UpdateResource ur;
  ur.worldMouse = GetScreenToWorld2D(mouse, context->camera);

  DrawResource dr;
  dr.font = context->getFont();
  dr.fontSize = 16;
  dr.mousePos = mouse;
  dr.worldMouse = GetScreenToWorld2D(mouse, context->camera);

  for (auto n : nodes) {
    n->update(ur);
    n->draw(dr);
  }
}

void NodeEditor::drawForeGround() {
  if (context->showContextMenu) {
    cr::ContextMenu<NodeType>();
  }
  controls();
  char buff[8];
  snprintf(buff, 8, "%d", GetFPS());
  DrawTextEx(context->getFont(), buff, {25, 25}, 16, 1.0F, GREEN);
}

void NodeEditor::controls() {
  auto& camera = context->camera;
  auto& dragStart = context->dragStart;
  auto& isDragging = context->isDragging;
  auto mouse = GetMousePosition();

  camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  camera.zoom += GetMouseWheelMove() * 0.05f;
  camera.zoom = (camera.zoom > 3.0f) ? 3.0f : camera.zoom;
  camera.zoom = (camera.zoom < 0.1f) ? 0.1f : camera.zoom;

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    context->contextMenuPos = GetMousePosition();
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    auto& menu = context->contextMenuPos;
    if (mouse.x == menu.x && mouse.y == menu.y) {
      context->showContextMenu = !context->showContextMenu;
      menu.x -= 3;
      menu.y -= 3;
      context->isDragging = false;
      return;
    }
  }

  // Panning with right mouse button
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    dragStart = GetScreenToWorld2D(GetMousePosition(), camera);
    isDragging = true;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    isDragging = false;
  }

  if (isDragging) {
    Vector2 dragEnd = GetScreenToWorld2D(GetMousePosition(), camera);
    camera.target.x -= (dragEnd.x - dragStart.x);
    camera.target.y -= (dragEnd.y - dragStart.y);
    dragStart = GetScreenToWorld2D(GetMousePosition(), camera);
  }

  if(IsKeyDown(KEY_B)){
    context->createNode(NodeType::HEADER, {(float)GetRandomValue(0, 750), (float)GetRandomValue(0, 750)});
    printf("%d", (int)context->UID);
  }
}
