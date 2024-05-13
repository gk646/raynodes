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
#include "ui/Window.h"

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

  static constexpr float CONTEXT_MENU_THRESHOLD = 15.0F;
  static constexpr float UI_SPACE_W = 1920.0F;  // UI space width
  static constexpr float UI_SPACE_H = 1080.0F;  // UI space height
  static constexpr float PAD = 25.0F;           // UI space padding amount

  ContextMenu contextMenu;
  std::vector<Window*> windows;  // UI Windows

  // General State
  bool showUnsavedChanges = false;

  // Dropdowns
  bool fileMenuState = false;  // FileMenu dropdown state
  bool editMenuState = false;  // EditMenu dropdown state
  bool viewMenuState = false;  // ViewMenu dropdown state

  // UI Elements
  bool showActionHistory = true;
  bool showMiniMap = true;

  // Settings
  bool showTopBarOnlyOnHover = false;
  bool showGrid = true;

  UI();

  // UI wrappers
  static int DrawListMenu(EditorContext& ec, bool& open, const char* title, const char* listText, int& active);
  static int DrawButton(EditorContext& ec, Rectangle& r, const char* txt);
  static int DrawButton(EditorContext& ec, Vector2& pos, float w, float h, const char* txt);
  static int DrawWindow(EditorContext& ec, const Rectangle& r, const char* txt);
  static const char* DrawTextPopUp(EditorContext& ec, const Rectangle& r, const char* text, bool& visible);
  // If icns is set it will look for icons in the string
  static void DrawText(EditorContext& ec, Vector2 p, const char* txt, Color c = COLORS[UI_LIGHT], bool icns = false);
  static void DrawRect(EditorContext& ec, Rectangle rec, int borderWidth, Color borderColor, Color color);

  // Allows to retrive the windows as both base class pointer and its direct class
  template <class T = Window>
  T* getWindow(WindowType type) {
    Window* window = nullptr;
    for (const auto w : windows) {
      if (w->getType() == type) {
        window = w;
        break;
      }
    }

    if (window == nullptr) return nullptr;
    if constexpr (std::is_same_v<T, Window>) {
      return window;
    } else {
      return static_cast<T*>(window);
    }
  }

  template <bool isBig = true>
  constexpr static Rectangle GetCenteredWindowBounds() {
    if constexpr (isBig) {
      constexpr auto winX = 650.0F;
      constexpr auto winW = 640.0F;
      constexpr auto winY = 350.0F;
      constexpr auto winH = 380.0F;

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