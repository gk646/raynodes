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

namespace {
void HandleNewNode(EditorContext& ec, Vector2 pos, const char* name) {
  const auto newN = ec.core.createNode(ec, name, GetScreenToWorld2D(pos, ec.display.camera));
  if (!newN) return;
  const auto action = new NodeCreateAction(2);
  action->createdNodes.push_back(newN);
  ec.core.addEditorAction(ec, action);
}
}  // namespace

void CanvasContextMenu::draw(EditorContext& ec, const Vector2& pos) {
  if (!ec.logic.showCanvasContextMenu) return;
  constexpr float menuWidth = 170.0F;
  const auto& font = ec.display.editorFont;
  const auto mouse = ec.logic.mouse;
  const auto fs = ec.display.fontSize;
  const auto hightLightColor = ColorAlpha(UI::COLORS[UI_LIGHT], 0.65);
  const auto padding = fs * 0.3F;
  const auto height = (fs + padding) * static_cast<float>(categories.size());
  const Rectangle menuRect = {pos.x, pos.y, menuWidth, height};
  Vector2 drawPos{pos.x, pos.y};

  // Draw main menu background
  DrawRectangleRounded(menuRect, 0.1F, 10, ColorAlpha(UI::COLORS[UI_DARK], 0.9));

  const Category* hoveredCategory = nullptr;
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

    DrawTextEx(font, category.name, {drawPos.x + padding, drawPos.y + padding / 2.0F}, fs, 0.6F,
               UI::COLORS[UI_LIGHT]);

    // Draw the category's nodes if it's open
    if (category.isOpen && !category.nodes.empty()) {
      Vector2 drawPosC = {drawPos.x + menuWidth, drawPos.y};
      DrawRectangleRounded(categoryRect, 0.1F, 10, ColorAlpha(UI::COLORS[UI_DARK], 0.9));
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
        DrawTextEx(font, name, {drawPosC.x + padding, drawPosC.y + padding / 2.0F}, fs, 0.7F, UI::COLORS[UI_LIGHT]);
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
    ec.logic.showCanvasContextMenu = false;
  }
}
void CanvasContextMenu::addNode(const char* category, const char* name) {
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
void CanvasContextMenu::removeNode(const char* category, const char* name) {
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