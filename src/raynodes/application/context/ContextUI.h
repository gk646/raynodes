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
  std::vector<ContextMenuCategory> categories;
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

struct UserInterface {
  ContextMenu contextMenu;
  const char* fileMenuText = "#001#File;#008#New;#005#Open;#002#Save;#006#Save As;#159#Exit";
  const char* editMenuText =
      "#022#Edit;#072#Undo;#073#Redo;#017#Cut;#016#Copy;#018#Paste;#143#Erase;#099#Select All";
  const char* viewMenuText = "#044#View;#220#Zoom In;#221#Zoom Out;#107#Zoom to Fit;#097#Grid";
  float topBarHeight = 18;
  bool showTopBarOnlyOnHover = true;
  bool showGrid = true;
  bool showContinueWithouSave = false;
  bool fileMenuState = false;  // FileMenu dropdown state
  bool editMenuState = false;  // EditMenu dropdown state
  bool viewMenuState = false;  // ViewMenu dropdown state

  void invokeFileMenu(EditorContext& ec, int i);
  void invokeEditMenu(EditorContext& ec, int i);
  void invokeViewMenu(EditorContext& ec, int i);
};

#endif  //CONTEXTUI_H