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

#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_

namespace Editor {
inline void DrawGrid(const EditorContext& ec) {
  if (!ec.ui.showGrid) return;
  const float baseGridSpacing = ec.display.gridSpacing;
  const auto& camera = ec.display.camera;
  // Calculate the visible world area
  Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D({ec.display.screenSize.x, ec.display.screenSize.y}, camera);

  // Calculate the starting points for drawing grid lines, adjusted for zoom
  float startX = std::floor(topLeft.x / baseGridSpacing) * baseGridSpacing;
  float startY = std::floor(topLeft.y / baseGridSpacing) * baseGridSpacing;

  // Calculate the end points for drawing grid lines, adjusted for zoom
  float endX = std::ceil(bottomRight.x / baseGridSpacing) * baseGridSpacing;
  float endY = std::ceil(bottomRight.y / baseGridSpacing) * baseGridSpacing;

  // Draw vertical grid lines
  for (float x = startX; x <= endX; x += baseGridSpacing) {
    // Convert world coordinates to screen coordinates for drawing
    Vector2 start = GetWorldToScreen2D({x, topLeft.y}, camera);
    Vector2 end = GetWorldToScreen2D({x, bottomRight.y}, camera);
    DrawLineV(start, end, UI::COLORS[E_GRID]);
  }

  // Draw horizontal grid lines
  for (float y = startY; y <= endY; y += baseGridSpacing) {
    // Convert world coordinates to screen coordinates for drawing
    Vector2 start = GetWorldToScreen2D({topLeft.x, y}, camera);
    Vector2 end = GetWorldToScreen2D({bottomRight.x, y}, camera);
    DrawLineV(start, end, UI::COLORS[E_GRID]);
  }
}
inline void DrawContextMenus(EditorContext& ec) {

  if (ec.ui.nodeCreateMenu.isVisible) {
    ec.ui.nodeCreateMenu.draw(ec, ec.logic.contextMenuPos);
  } else if (ec.ui.nodeContextMenu.isVisible) {
    ec.ui.nodeContextMenu.draw(ec, ec.logic.contextMenuPos);
  } else if (ec.ui.nodeGroupContextMenu.isVisible) {
    ec.ui.nodeGroupContextMenu.draw(ec, ec.logic.contextMenuPos);
  } else if (ec.ui.canvasContextMenu.isVisible) {
    ec.ui.canvasContextMenu.draw(ec, ec.logic.contextMenuPos);
  }
}
inline void DrawActions(EditorContext& ec) {
  constexpr int visibleActions = 10;  // Number of actions displayed at a time

  // Cache
  const auto& font = ec.display.editorFont;
  auto rect = ec.display.getFullyScaled({1720.0F, 50.0F, 180.0F, 230.0F});
  const float entryHeight = rect.height / visibleActions;
  const float fs = entryHeight * 0.8F;        // Allocating 80% of entry height to font size
  const float paddingY = entryHeight * 0.2F;  // Allocating 20% of entry height to padding
  const auto paddingX = rect.width * 0.03F;
  const auto currActionIdx = ec.core.currentActionIndex;

  // Adjust the start index to ensure the current action is always on screen
  const int totalActions = static_cast<int>(ec.core.actionQueue.size());
  const int start = std::max(0, std::min(currActionIdx, totalActions - visibleActions));
  const int end = std::min(start + visibleActions - 1, totalActions - 1);

  rect.height += paddingY * 2.0F;  // We start with the same offset

  DrawRectangleRec(rect, ColorAlpha(UI::COLORS[UI_DARK], 0.85F));
  DrawRectangleLinesEx(rect, paddingX / 4.0F, ColorAlpha(UI::COLORS[UI_MEDIUM], 0.7F));

  rect.y += paddingY;

  //Coloring scheme is inspired by paint.net
  for (int i = start; i <= end; ++i) {
    auto* action = ec.core.actionQueue[i];
    const bool isCurrentAction = i == currActionIdx;
    const bool isCurrentOrBelow = i >= currActionIdx;
    const bool isCurrentOrAbove = i <= currActionIdx;

    // All text is white expect past actions
    const Color textColor = isCurrentOrAbove ? UI::COLORS[UI_LIGHT] : UI::COLORS[E_GRID];

    // Only current events is blue, past ones gray
    const Color highLightColor = isCurrentAction ? UI::COLORS[UI_LIGHT] : UI::COLORS[UI_MEDIUM];

    // Draw background highlight for the current action
    if (isCurrentOrBelow) {
      DrawRectangleRec({rect.x + paddingX, rect.y, rect.width - 2 * paddingX, fs + paddingY},
                       ColorAlpha(highLightColor, 0.4F));
    }

    // Draw action text
    DrawTextEx(font, action->toString(), {rect.x + paddingX * 2.0F, rect.y + paddingY / 2}, fs, 1.0F, textColor);

    // Move to the next action's position, ensuring space for text and padding
    rect.y += entryHeight;
  }
}
inline void DrawTopBar(EditorContext& ec) {
  //TODO improve dropdown - like paint.net have a small button that expands to a bigger window
  // Dont make the base dropdown as big as the expanded categories
  // Dont draw the base category

  constexpr auto dropDown = [](EditorContext& ec, Rectangle& r, const char* text, bool& state) {
    // Always reset to 0 cause its a dropdown
    int active = 0;
    // We use extra getter to properly scale it
    const auto bounds = ec.display.getSmartScaled(r);
    r.x += bounds.width;
    if (GuiDropdownBox(bounds, text, &active, state)) {
      state = !state;
      ec.input.consumeMouse();
      ec.logic.isDraggingScreen = false;
      return active;
    }
    if (state) { ec.input.consumeMouse(); }
    return -1;
  };

  const auto height = 20;
  const auto width = 140.0F;
  auto bounds = Rectangle{20, 5, width, height};

  int res = -1;
  res = dropDown(ec, bounds, UI::fileMenuText, ec.ui.fileMenuState);
  UI::invokeFileMenu(ec, res);

  res = dropDown(ec, bounds, UI::editMenuText, ec.ui.editMenuState);
  UI::invokeEditMenu(ec, res);

  res = dropDown(ec, bounds, UI::viewMenuText, ec.ui.viewMenuState);
  UI::invokeViewMenu(ec, res);

  auto buttonBounds = Rectangle{1860, 5.0F, 24.0F, 24.0F};
  if (UI::DrawButton(ec, buttonBounds, "#141#")) ec.ui.getWindow(SETTINGS_MENU)->toggleWindow(ec);
  buttonBounds.x += 24.0F;
  if (UI::DrawButton(ec, buttonBounds, "#193#")) ec.ui.getWindow(HELP_MENU)->toggleWindow(ec);

  // Draw tool toggle buttons
}
inline void DrawStatusBar(EditorContext& ec) {
  constexpr float height = 20.0F;
  constexpr float y = 1080 - height;
  const char* text;
  // Status bar draw func
  constexpr auto statusBar = [](EditorContext& ec, float& x, float y, float w, float h, const char* txt) {
    GuiStatusBar(ec.display.getFullyScaled({x, y, w, h}), txt);
    x += w;
  };

  // Draw base bar
  GuiStatusBar(ec.display.getFullyScaled({0, y, 1920.0F, height}), "");

  // Left panels
  {
    float leftPanels = 0.0F;
    text = ec.string.formatText("#098# Nodes: %d", (int)ec.core.nodes.size());
    statusBar(ec, leftPanels, y, 150.0F, height, text);
    text = ec.string.formatText("#070# Connections: %d", (int)ec.core.connections.size());
    statusBar(ec, leftPanels, y, 180.0F, height, text);
  }

  // Right panels
  {
    // Show mouse position
    float rightPanels = 1200.0F;
    text = ec.string.formatText("#021# %d/%d", (int)ec.logic.worldMouse.x, (int)ec.logic.worldMouse.y);
    statusBar(ec, rightPanels, y, 150.0F, height, text);

    const auto lastWidth = 1920.0F - rightPanels;
    statusBar(ec, rightPanels, y, lastWidth, height, nullptr);

    // Draw the Zoom slider
    const auto bounds = ec.display.getFullyScaled({rightPanels - lastWidth + 25, y, lastWidth - 50, height});
    if (GuiSliderBar(bounds, nullptr, nullptr, &ec.display.camera.zoom, 0.1F, 3.0F)) {
      ec.input.consumeMouse();
    } else if (ec.input.isMBPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(ec.logic.mouse, bounds)) {
      ec.input.consumeMouse();
    }

    // Draw zoom labels
    if (GuiButton(ec.display.getFullyScaled({rightPanels - lastWidth, y, 25, height}), "#221#")) {
      ec.display.zoomOut();
    }
    if (GuiButton(ec.display.getFullyScaled({rightPanels - 25, y, 25, height}), "#220#")) { ec.display.zoomIn(); }
  }
}
inline void DrawUnsavedChanges(EditorContext& ec) {
  if (!ec.ui.showUnsavedChanges) return;

  auto windowRect = UI::GetCenteredWindowBounds<false>();  // Centered rect in ui space

  // Get information string
  const char* fileName = GetFileName(ec.persist.openedFilePath.c_str());
  if (fileName == nullptr || *fileName == '\0') fileName = "Untitled.rn";
  const char* infoText = ec.string.formatText("%s has unsaved changes", fileName);
  if (UI::DrawWindow(ec, windowRect, infoText)) { ec.ui.showUnsavedChanges = false; }

  constexpr float buttonHeight = 50.0F;
  windowRect.y = windowRect.y + 30;
  windowRect.height = buttonHeight;

  if (UI::DrawButton(ec, windowRect, "#002#Save")) {
    ec.persist.saveProject(ec);
    ec.ui.showUnsavedChanges = false;
    //TODO respect user intent and execute next action -> need to save how user got here
  }
  windowRect.y += buttonHeight;
  if (UI::DrawButton(ec, windowRect, "#159#Don't Save")) {
    ec.ui.showUnsavedChanges = false;
    ec.core.hasUnsavedChanges = false;
    if (ec.core.requestedClose) ec.core.closeApplication = true;
  }
  windowRect.y += buttonHeight;
  if (UI::DrawButton(ec, windowRect, "#072#Cancel")) { ec.ui.showUnsavedChanges = false; }
}
inline void DrawSideBar(EditorContext& ec) {
  Rectangle button = {5, 250, 100, 25};
  if (GuiButton(ec.display.getFullyScaled(button), "Custom Nodes")) {
    ec.ui.getWindow(NODE_CREATOR)->toggleWindow(ec);
  }
}
inline void DrawWindows(EditorContext& ec) {
  for (const auto w : ec.ui.windows) {
    w->draw(ec);
  }
}
}  // namespace Editor

#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_