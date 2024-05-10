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

#ifndef CONTEXTUI_H
#define CONTEXTUI_H

struct ContextMenuCategory {
  const char* name;
  std::vector<const char*> nodes;
  bool isOpen = false;
};

struct ContextMenu {
  std::vector<ContextMenuCategory> categories{};
  void addNode(const char* category, const char* name) {
    for (auto& c : categories) {
      if (cxstructs::str_cmp(c.name, category)) {
        c.nodes.push_back(name);
        return;
      }
    }
    ContextMenuCategory cat{category, {}, false};
    cat.nodes.push_back(name);
    categories.push_back(cat);
  }
};

// All the ui is made to be normed to 1920x1080 so FullHD
// The methods allow you to always us absolute coordinates and then transferthem to real screen space
// Use display.smartScaling for elements that have a minimum height -> scaling will only be apl

struct UI final {
  static constexpr Color COLORS[INDEX_END] = {
      {45, 45, 45, 255},     // E_BACK_GROUND
      {27, 27, 27, 255},     // E_GRID
      {63, 63, 63, 255},     // N_BACK_GROUND
      {207, 207, 207, 255},  // UI_LIGHT
      {88, 88, 88, 255},     // UI_MEDIUM
      {60, 60, 60, 255}      // UI_DARK
  };

  static constexpr auto* fileMenuText = "#001#File;"
                                        "#008#New (Ctrl+N);"
                                        "#005#Open (Ctrl+O);"
                                        "#002#Save (Ctrl+S);"
                                        "#006#Save As (Ctrl+Shift+S);"
                                        "#159#Exit";

  static constexpr auto* editMenuText = "#022#Edit;"
                                        "#072#Undo (Ctrl+Z);"
                                        "#073#Redo (Ctrl+Y);"
                                        "#017#Cut (Ctrl+X);"
                                        "#016#Copy (Ctrl+C);"
                                        "#018#Paste (Ctrl+V);"
                                        "#143#Erase (Del);"
                                        "#099#Select All (Ctrl+A)";

  static constexpr auto* viewMenuText = "#044#View;"
                                        "#220#Zoom In (Ctrl++);"
                                        "#221#Zoom Out (Ctrl+-);"
                                        "#107#Zoom to Fit;"
                                        "#097#Grid";

  static constexpr auto* settingsMenuText = "#181#User Interface;"
                                            "#222#Updates";

  static constexpr auto* helpMenuText = "#225#Wiki;"
                                        "#224#Github;"
                                        "#191#About";

  static constexpr float CONTEXT_MENU_THRESHOLD = 15.0F;
  static constexpr float UI_SPACE_W = 1920.0F;  // UI space width
  static constexpr float UI_SPACE_H = 1080.0F;  // UI space height
  static constexpr float PAD = 25.0F;           // UI space padding amount

  ContextMenu contextMenu;
  float topBarHeight = 20;
  ScaleDirection scaleDirection = HORIZONTAL;  // Used by ui functions to automatically apply correct offset
  int settingsScrollIndex = 0;
  int settingsActiveIndex = 0;
  int helpScrollIndex = 0;
  int helpActiveIndex = 0;

  bool showTopBarOnlyOnHover = true;
  bool showGrid = true;
  bool showUnsavedChanges = false;
  bool fileMenuState = false;  // FileMenu dropdown state
  bool editMenuState = false;  // EditMenu dropdown state
  bool viewMenuState = false;  // ViewMenu dropdown state
  bool showSettingsMenu = false;
  bool showHelpMenu = false;

  // UI wrappers
  static int DrawListMenu(EditorContext& ec, bool& open, const char* title, const char* listText, int& active);
  static int DrawButton(EditorContext& ec, Rectangle& r, const char* txt);
  static int DrawButton(EditorContext& ec, Vector2& pos, float w, float h, const char* txt);
  static int DrawWindow(EditorContext& ec, const Rectangle& r, const char* txt);
  // If icns is set it fill look for icons in the string
  static void DrawText(EditorContext& ec, Vector2 p, const char* txt, Color c = COLORS[UI_LIGHT], bool icns = false);
  template <bool isBig= true>
  constexpr static Rectangle GetCenteredWindow() {
    if constexpr (isBig) {
      constexpr auto winX = 700.0F;
      constexpr auto winW = 540.0F;
      constexpr auto winY = 400.0F;
      constexpr auto winH = 280.0F;

      return Rectangle{winX, winY, winW, winH};
    }

    constexpr auto winX = 790.0F;
    constexpr auto winW = 360.0F;
    constexpr auto winY = 450.0F;
    constexpr auto winH = 180.0F;

    return Rectangle{winX, winY, winW, winH};
  }

  // Menu functions
  static void invokeFileMenu(EditorContext& ec, int i);
  static void invokeEditMenu(EditorContext& ec, int i);
  static void invokeViewMenu(EditorContext& ec, int i);
  static void invokeHelpMenu(EditorContext& ec, int i);
  static void invokeSettingsMenu(EditorContext& ec, int i);
};

#endif  //CONTEXTUI_H