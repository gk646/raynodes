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

#include "application/EditorContext.h"
#include "ui/elements/CanvasContextMenu.h"
#include "application/elements/Action.h"

#include "shared/rayutils.h"
#include "shared/uiutil.h"

namespace {
void HandleNewNode(EditorContext& ec, const Vector2 pos, const char* name) {
  const auto newN = ec.core.createNode(ec, name, GetScreenToWorld2D(pos, ec.display.camera));
  if (!newN) return;
  const auto action = new NodeCreateAction(2);
  action->createdNodes.push_back(newN);
  ec.core.addEditorAction(ec, action);
}

template <typename Iterable>
bool DrawList(EditorContext& ec, const Vector2 pos, const Iterable& names) {
  const auto released = ec.input.isMouseButtonReleased(MOUSE_BUTTON_LEFT);

  const auto& f = ec.display.editorFont;
  const auto fs = ec.display.fontSize;
  const auto mouse = ec.logic.mouse;
  const auto entryHeight = fs + 4;

  Rectangle entry = {pos.x, pos.y, 180, 20};

  // Colors
  const auto background = ColorAlpha(UI::COLORS[UI_DARK], 0.97F);
  const auto highlight = ColorAlpha(UI::COLORS[UI_MEDIUM], 0.97F);

  bool oneHovered = false;

  for (const auto name : names) {
    if (CheckCollisionPointRec(mouse, entry)) {
      DrawRectangleRounded(entry, 0.1F, 30, highlight);
      if (released) {
        HandleNewNode(ec, ec.logic.contextMenuPos, name);
        ec.logic.showCanvasContextMenu = false;
        return false;
      }
      oneHovered = true;
    } else {
      DrawRectangleRec(entry, background);
    }
    const auto textPos = Vector2{entry.x, entry.y + 2};
    constexpr auto color = UI::COLORS[UI_LIGHT];
    DrawTextEx(f, name, textPos, fs, 0.0F, color);
    entry.y += entryHeight;
  }

  const bool isClose =
      CheckExtendedRec(mouse, {pos.x, pos.y, 180, names.size() * entryHeight}, UI::CONTEXT_MENU_THRESHOLD);
  return oneHovered || isClose;
}
}  // namespace

void CanvasContextMenu::draw(EditorContext& ec, const Vector2 pos) {
  handleOpen();
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) ec.input.consumeMouse();

  // Cache
  const auto mouse = ec.logic.mouse;
  Rectangle entry = {pos.x, pos.y, 180, 20};

  // Textfield
  searchBar.bounds = entry;
  searchBar.draw("Search...");
  searchBar.update(ec, mouse);

  if (searchBar.buffer.empty()) drawCategories(ec, {pos.x, pos.y + 20});
  else {
    SortVector sorted;
    auto searchStr = searchBar.buffer.c_str();
    for (const auto& cat : categories) {
      for (const auto name : cat.nodes) {
        StringFilter(name, searchStr, sorted, [](const char* arg) { return arg; });
      }
    }
    SortFilteredVector(sorted, searchStr, [](const char* arg) { return arg; });
    ec.logic.showCanvasContextMenu = DrawList(ec, {pos.x, pos.y + 20}, sorted);
    prevState = ec.logic.showCanvasContextMenu;
  }
}

void CanvasContextMenu::drawCategories(EditorContext& ec, Vector2 pos) {
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
    } else if (isOpen) {
      DrawRectangleRounded(entry, 0.1F, 30, highlight);
    } else {
      DrawRectangleRec(entry, background);
      isOpen = false;
    }

    if (isOpen) {
      insideCategory = DrawList(ec, {pos.x + 180, entry.y}, nodes);  // Draw subcategories
    }

    const Vector2 textPos = {entry.x, entry.y + 2};
    constexpr auto color = UI::COLORS[UI_LIGHT];
    DrawTextEx(f, name, textPos, fs, 0.0F, color);
    entry.y += entryHeight;
  }

  const auto extendedRec = Rectangle{pos.x, pos.y, 180, categories.size() * entryHeight + 20};
  if (!(insideCategory || CheckExtendedRec(mouse, extendedRec, UI::CONTEXT_MENU_THRESHOLD))) {
    ec.logic.showCanvasContextMenu = false;
  }
  prevState = ec.logic.showCanvasContextMenu;
}

void CanvasContextMenu::handleOpen() {
  if (prevState == false) {
    searchBar.buffer.clear();
    for (auto& cat : categories) {
      cat.isOpen = false;
    }
    prevState = true;
  }
}

void CanvasContextMenu::addEntry(const char* category, const char* name) {
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

void CanvasContextMenu::removeEntry(const char* category, const char* name) {
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