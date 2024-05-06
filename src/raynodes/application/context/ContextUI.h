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

// E = Editor
// N = Node
// C = Component
enum ColorIndex : uint8_t {
  E_BACK_GROUND = 0,
  E_GRID,
  N_BACK_GROUND,
  UI_LIGHT,
  UI_MEDIUM,
  UI_DARK,
  INDEX_END
};

struct UI {
  ContextMenu contextMenu;
  static constexpr Color COLORS[INDEX_END] = {
      {45, 45, 45, 255},     // E_BACK_GROUND
      {27, 27, 27, 255},     // E_GRID
      {63, 63, 63, 255},     // N_BACK_GROUND
      {207, 207, 207, 255},  // UI_LIGHT
      {88, 88, 88, 255},     // UI_MEDIUM
      {56, 56, 56, 255}      // UI_DARK
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
  float topBarHeight = 20;
  bool showTopBarOnlyOnHover = true;
  bool showGrid = true;
  bool showUnsavedChanges = false;
  bool fileMenuState = false;  // FileMenu dropdown state
  bool editMenuState = false;  // EditMenu dropdown state
  bool viewMenuState = false;  // ViewMenu dropdown state

  static void invokeFileMenu(EditorContext& ec, int i);
  static void invokeEditMenu(EditorContext& ec, int i);
  static void invokeViewMenu(EditorContext& ec, int i);
};

#endif  //CONTEXTUI_H