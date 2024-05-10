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

namespace {
void HandleNewNode(EditorContext& ec, Vector2 pos, const char* name) {
  const auto newN = ec.core.createNode(ec, name, GetScreenToWorld2D(pos, ec.display.camera));
  if (!newN) return;
  const auto action = new NodeCreateAction(2);
  action->createdNodes.push_back(newN);
  ec.core.addEditorAction(ec, action);
}
}  // namespace

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
inline void DrawContextMenu(EditorContext& ec) {
  if (!ec.logic.showContextMenu) return;
  constexpr float menuWidth = 170.0F;
  auto& categories = ec.ui.contextMenu.categories;
  const auto& font = ec.display.editorFont;
  const auto mouse = ec.logic.mouse;
  const auto fs = ec.display.fontSize;
  const auto pos = ec.logic.contextMenuPos;
  const auto hightLightColor = ColorAlpha(WHITE, 0.35);
  const auto padding = fs * 0.3F;
  const auto height = (fs + padding) * static_cast<float>(categories.size());
  const Rectangle menuRect = {pos.x, pos.y, menuWidth, height};
  Vector2 drawPos{pos.x, pos.y};

  // Draw main menu background
  DrawRectangleRounded(menuRect, 0.1F, 10, ColorAlpha(UI::COLORS[UI_DARK], 0.8));

  const ContextMenuCategory* hoveredCategory = nullptr;
  bool closeToMenu = false;

  for (auto& category : categories) {
    const Rectangle textRect = {drawPos.x, drawPos.y, menuWidth, fs + padding};
    float categoryHeight = static_cast<float>(category.nodes.size()) * (fs + padding);
    Rectangle categoryRect = {drawPos.x + menuWidth, drawPos.y, menuWidth, categoryHeight};

    // Detect if mouse is over this category
    if (CheckCollisionPointRec(mouse, textRect)
        || (CheckCollisionPointRec(mouse, categoryRect) && category.isOpen)) {
      if (hoveredCategory != nullptr) {
        for (auto& category2 : categories) {
          category2.isOpen = false;
        }
      }
      hoveredCategory = &category;  // Update the currently hovered category

    } else {
      if (CheckExtendedRec(mouse, textRect, UI::CONTEXT_MENU_THRESHOLD)) {
        closeToMenu = true;  // If menu closes too fast its annoying
      } else if (CheckExtendedRec(mouse, categoryRect, UI::CONTEXT_MENU_THRESHOLD) && category.isOpen) {
        closeToMenu = true;  // If menu closes too fast its annoying
        hoveredCategory = &category;
      }
    }

    category.isOpen = hoveredCategory == &category;

    // Highlight the category if it's open
    if (category.isOpen) { DrawRectangleRounded(textRect, 0.1F, 10, hightLightColor); }

    DrawTextEx(font, category.name, {drawPos.x + padding, drawPos.y + padding}, fs, 0.6F, UI::COLORS[UI_LIGHT]);

    // Draw the category's nodes if it's open
    if (category.isOpen && !category.nodes.empty()) {
      Vector2 drawPosC = {drawPos.x + menuWidth, drawPos.y};
      DrawRectangleRounded(categoryRect, 0.1F, 10, ColorAlpha(UI::COLORS[UI_DARK], 0.8));
      for (const auto& name : category.nodes) {
        const Rectangle nodeTextRect = {drawPosC.x, drawPosC.y, menuWidth, fs + padding};
        if (CheckCollisionPointRec(mouse, nodeTextRect)) {
          DrawRectangleRounded(nodeTextRect, 0.1F, 10, hightLightColor);
          if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            HandleNewNode(ec, pos, name);
            hoveredCategory = nullptr;
            break;
          }
        }
        DrawTextEx(font, name, {drawPosC.x + padding, drawPosC.y + padding}, fs, 0.7F, UI::COLORS[UI_LIGHT]);
        drawPosC.y += fs + padding;
      }
    }

    drawPos.y += fs + padding;
  }

  // If no category is hovered, close all and hide the context menu
  if (!hoveredCategory && !closeToMenu) {
    for (auto& category : categories) {
      category.isOpen = false;
    }
    ec.logic.showContextMenu = false;
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

  const auto height = ec.ui.topBarHeight;
  const auto width = 140.0F;
  auto bounds = Rectangle{20, 5, width, height};

  int res = -1;
  res = dropDown(ec, bounds, UI::fileMenuText, ec.ui.fileMenuState);
  UI::invokeFileMenu(ec, res);

  res = dropDown(ec, bounds, UI::editMenuText, ec.ui.editMenuState);
  UI::invokeEditMenu(ec, res);

  res = dropDown(ec, bounds, UI::viewMenuText, ec.ui.viewMenuState);
  UI::invokeViewMenu(ec, res);

  ec.ui.scaleDirection = HORIZONTAL;
  auto buttonBounds = Rectangle{1860, 5.0F, 24.0F, 24.0F};
  if (UI::DrawButton(ec, buttonBounds, "#141#")) ec.ui.showSettingsMenu = !ec.ui.showSettingsMenu;

  if (UI::DrawButton(ec, buttonBounds, "#193#")) ec.ui.showHelpMenu = !ec.ui.showHelpMenu;
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
    } else if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(ec.logic.mouse, bounds)) {
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

  auto windowRect = UI::GetCenteredWindow<false>();  // Centered rect in ui space

  // Get information string
  const char* fileName = GetFileName(ec.persist.openedFilePath.c_str());
  if (fileName == nullptr || *fileName == '\0') fileName = "Untitled.rn";
  const char* infoText = ec.string.formatText("%s has unsaved changes", fileName);
  if (UI::DrawWindow(ec, windowRect, infoText)) { ec.ui.showUnsavedChanges = false; }

  windowRect.y = windowRect.y + 30;
  windowRect.height = 50.0F;

  ec.ui.scaleDirection = VERTICAL;
  if (UI::DrawButton(ec, windowRect, "#002#Save")) {
    ec.persist.saveToFile(ec);
    ec.ui.showUnsavedChanges = false;
    //TODO respect user intent and execute next action -> need to save how user got here
  }
  if (UI::DrawButton(ec, windowRect, "#159#Don't Save")) {
    ec.ui.showUnsavedChanges = false;
    ec.core.hasUnsavedChanges = false;
    if (ec.core.requestedClose) ec.core.closeApplication = true;
  }
  if (UI::DrawButton(ec, windowRect, "#072#Cancel")) { ec.ui.showUnsavedChanges = false; }
}
inline void DrawSettingsMenu(EditorContext& ec) {
  auto r = UI::DrawListMenu(ec, ec.ui.showSettingsMenu, "Settings", UI::settingsMenuText, ec.ui.settingsActiveIndex);
  auto winBounds = UI::GetCenteredWindow();
  Vector2 topLeft = {winBounds.x + 150.0F + UI::PAD, winBounds.y + UI::PAD};  // Hardcoded from DrawListMenu

  if (r == 0) {  // User Interface
    UI::DrawText(ec, topLeft, "Here will be UI options");
  } else if (r == 1) {  // Updates
    UI::DrawText(ec, topLeft, "Here will be automatic updates");
  }
}
inline void DrawHelpMenu(EditorContext& ec) {
  auto r = UI::DrawListMenu(ec, ec.ui.showHelpMenu, "Help", UI::helpMenuText, ec.ui.helpActiveIndex);
  auto winBounds = UI::GetCenteredWindow();
  Vector2 topLeft = {winBounds.x + 150.0F + UI::PAD, winBounds.y + UI::PAD};  // Hardcoded from DrawListMenu

  if (r == 0) {  // Wiki
    if (UI::DrawButton(ec, topLeft, 25, 25, "Open the Wiki")) { OpenURL(Info::wikiLink); }
  } else if (r == 1) {  // Github
    if (UI::DrawButton(ec, topLeft, 25, 25, "Open the github page")) { OpenURL(Info::github); }
  } else if (r == 2) {  // About
    ec.string.formatText("%s %s", Info::applicationName, Info::getVersion(ec));
    UI::DrawText(ec, topLeft, ec.string.buffer);
    topLeft.y += UI::PAD;
    UI::DrawText(ec, topLeft, Info::about, UI::COLORS[UI_LIGHT], true);
  }
}
}  // namespace Editor

#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITORUI_H_