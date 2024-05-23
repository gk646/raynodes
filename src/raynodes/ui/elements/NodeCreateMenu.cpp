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

#include <raylib.h>
#include <raygui.h>

#include "application/EditorContext.h"
#include "ui/elements/NodeCreateMenu.h"
#include "application/elements/Action.h"

#include "shared/rayutils.h"
#include "shared/uiutil.h"

namespace {
void HandleNewNode(EditorContext& ec, const Vector2 pos, const char* name) {
  const auto newN = ec.core.createAddNode(ec, name, GetScreenToWorld2D(pos, ec.display.camera));
  if (!newN) return;
  const auto action = new NodeCreateAction(2);
  action->createdNodes.push_back(newN);
  ec.core.addEditorAction(ec, action);
}
template <typename Iterable>
bool DrawList(EditorContext& ec, NodeCreateMenu& menu, const Vector2 pos, const Iterable& names) {
  const auto released = ec.input.isMBReleased(MOUSE_BUTTON_LEFT);

  // Create node on enter
  if (IsKeyPressed(KEY_ENTER) && names.size() == 1) {
    HandleNewNode(ec, ec.logic.contextMenuPos, names[0]);
    menu.isVisible = false;
    return false;
  }

  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto mouse = ec.logic.mouse;
  const auto entryHeight = fs + 4;
  const auto totalHeight = names.size() * entryHeight;

  Rectangle entry = {pos.x, pos.y, 180, 20};
  bool oneHovered = false;

  // Scroll
  const Rectangle scrollPanelRec = {pos.x, pos.y, 194, std::min(200.0f, totalHeight)};
  static Vector2 scroll = {0, 0};
  const Rectangle contentRec = {0, 0, 180, totalHeight};
  Rectangle view;
  GuiScrollPanel(scrollPanelRec, nullptr, contentRec, &scroll, &view);

  entry.y = pos.y + scroll.y;
  BeginScissorMode(view.x, view.y, view.width, view.height);
  {
    for (const auto name : names) {
      if (CheckCollisionPointRec(mouse, entry)) {
        DrawRectangleRounded(entry, 0.1F, 30, UI::Lighten(UI::COLORS[UI_MEDIUM], 15));
        if (released) {
          HandleNewNode(ec, ec.logic.contextMenuPos, name);
          menu.isVisible = false;
          EndScissorMode();
          return false;
        }
        oneHovered = true;
      } else {
        DrawRectangleRec(entry, UI::Lighten(UI::COLORS[UI_DARK]));
      }

      const auto textPos = Vector2{entry.x + 5, entry.y + 2};
      constexpr auto color = UI::COLORS[UI_LIGHT];
      DrawTextEx(f, name, textPos, fs, 0.0F, color);
      entry.y += entryHeight;
    }
  }
  EndScissorMode();
  const bool isClose = CheckExtendedRec(mouse, {pos.x, pos.y, 180, totalHeight}, UI::CONTEXT_MENU_THRESHOLD);
  return oneHovered || isClose;
}
}  // namespace

void NodeCreateMenu::draw(EditorContext& ec, const Vector2 pos) {
  handleOpen();

  // Textfield
  searchBar.bounds = {pos.x, pos.y, 180, 20};
  searchBar.draw("Search...");
  searchBar.update(ec, ec.logic.mouse);

  if (searchBar.buffer.empty()) drawCategories(ec, {pos.x, pos.y + 20});
  else {
    SortVector sorted;
    const auto* searchStr = searchBar.buffer.c_str();
    for (const auto& cat : categories) {
      for (const auto name : cat.nodes) {
        StringFilter(name, searchStr, sorted, [](const char* arg) { return arg; });
      }
    }
    SortFilteredVector(sorted, searchStr, [](const char* arg) { return arg; });
    const bool insideList = DrawList(ec, *this, {pos.x, pos.y + 20}, sorted);
    if (!insideList && ec.input.isMBPressed(MOUSE_BUTTON_LEFT)) { isVisible = false; }
    prevState = isVisible;
  }

  ec.input.consumeMouse();
}

void NodeCreateMenu::drawCategories(EditorContext& ec, Vector2 pos) {
  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto mouse = ec.logic.mouse;

  const auto entryHeight = fs + 4;

  // Colors
  const auto background = ColorAlpha(UI::COLORS[UI_DARK], 0.97F);
  const auto highlight = ColorAlpha(UI::COLORS[UI_MEDIUM], 0.97F);

  Rectangle entry = {pos.x, pos.y, 180, 20};

  bool insideCategory = false;
  for (auto& [name, nodes, isOpen] : categories) {
    if (CheckCollisionPointRec(mouse, entry)) {
      DrawRectangleRounded(entry, 0.1F, 30, highlight);
      if (!isOpen) {
        for (auto& c : categories) {
          c.isOpen = false;
        }
      }
      isOpen = true;
      insideCategory = true;
      DrawList(ec, *this, {pos.x + 180, entry.y}, nodes);
    } else if (isOpen) {
      DrawRectangleRounded(entry, 0.1F, 30, highlight);
      insideCategory = true;
      isOpen = DrawList(ec, *this, {pos.x + 180, entry.y}, nodes);
    } else {
      DrawRectangleRec(entry, background);
      isOpen = false;
    }
    DrawTextEx(f, name, {entry.x, entry.y + 2}, fs, 0.0F, UI::COLORS[UI_LIGHT]);
    entry.y += entryHeight;
  }

  const auto extendedRec = Rectangle{pos.x, pos.y - 20, 180, categories.size() * entryHeight + 20};
  if (!(insideCategory || CheckExtendedRec(mouse, extendedRec, UI::CONTEXT_MENU_THRESHOLD))) { isVisible = false; }
  prevState = isVisible;
}

void NodeCreateMenu::handleOpen() {
  if (prevState == false) {
    searchBar.buffer.clear();
    searchBar.onFocusGain({searchBar.bounds.x, searchBar.bounds.y});
    for (auto& cat : categories) {
      cat.isOpen = false;
    }
    prevState = true;
  }
}

void NodeCreateMenu::addEntry(const char* category, const char* name) {
  for (auto& c : categories) {
    if (cxstructs::str_cmp(c.name, category)) {
      c.nodes.push_back(name);
      return;
    }
  }

  Category cat{category, {}, false};
  cat.nodes.push_back(name);
  categories.push_back(cat);
}

void NodeCreateMenu::removeEntry(const char* category, const char* name) {
  for (auto catIt = categories.begin(); catIt != categories.end();) {
    if (cxstructs::str_cmp(catIt->name, category)) {
      bool erased = false;
      for (auto nodeIt = catIt->nodes.begin(); nodeIt != catIt->nodes.end();) {
        if (cxstructs::str_cmp(*nodeIt, name)) {
          nodeIt = catIt->nodes.erase(nodeIt);
          erased = true;
        } else {
          ++nodeIt;
        }
      }

      if (catIt->nodes.empty()) {
        catIt = categories.erase(catIt);
      } else {
        ++catIt;
      }
      if (erased) return;
    }
    ++catIt;
  }
}