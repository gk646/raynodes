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

#include <ranges>

#include "NodeCreator.h"
#include "application/EditorContext.h"

void DrawNodeCreateSandbox(EditorContext& ec, Rectangle space, const NodeTemplate* nTemplate, Node* node) {
  if (nTemplate == nullptr || node == nullptr) return;
  // TODO possibly draw the grid aswell / should look like the node editor
  // Draw BackGround
  DrawRectangleRec(ec.display.getFullyScaled(space), UI::COLORS[N_BACK_GROUND]);

  Node::Draw(ec, *node);
}

void NodeCreator::drawContent(EditorContext& ec, const Rectangle& body) {
  // Cache
  auto window = UI::GetCenteredWindowBounds<true>();
  const auto& font = ec.display.editorFont;
  const auto fs = ec.display.fontSize;

  // Draw Border
  const auto borderBounds = Rectangle{window.x, window.y + UI::PAD, 150, window.height - UI::PAD};
  UI::DrawRect(ec, borderBounds, 2, UI::COLORS[UI_DARK], BLANK);

  // Draw Create Button
  if (ec.templates.userDefinedNodes.empty()) {
    auto listBounds = Rectangle{window.x, window.y + UI::PAD, 150, 50};
    if (UI::DrawButton(ec, listBounds, "Create a custom node")) { showNamePopup = true; }

    if (showNamePopup) {
      const auto smallWindow = UI::GetCenteredWindowBounds<false>();
      auto name = UI::DrawTextPopUp(ec, smallWindow, "Choose a name for your node:", showNamePopup);
      if (name != nullptr) {
        NodeTemplate nTemplate;
        nTemplate.label = cxstructs::str_dup(name);  // Need a allocated copy
        ec.templates.registerUserNode(
            nTemplate, [](const NodeTemplate& nt, Vec2 pos, NodeID id) { return new Node(nt, pos, id); });
      }
    }

    return;
  }

  // Draw List Menu
  Rectangle entry = {window.x, window.y + UI::PAD, 150, 50};
  Rectangle space = {window.x + 150.0F, window.y + UI::PAD, window.width - 150.0F, window.height - UI::PAD};
  Vector2 topLeft = {space.x, entry.y};
  const NodeTemplate* activeTemplate = nullptr;

  int i = 0;
  for (const auto& nInfo : ec.templates.userDefinedNodes | std::ranges::views::values) {
    const bool selected = i == activeEntry;
    const auto text = selected ? UI::COLORS[UI_MEDIUM] : UI::COLORS[UI_LIGHT];
    const auto background = selected ? UI::COLORS[UI_LIGHT] : UI::COLORS[UI_MEDIUM];
    const auto border = UI::COLORS[UI_DARK];

    if (selected) activeTemplate = &nInfo.nTemplate;

    UI::DrawText(ec, {entry.x, entry.y + UI::PAD}, nInfo.nTemplate.label, text, false);
    UI::DrawRect(ec, entry, 2, border, background);
    if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) activeEntry = i;
    entry.y += 50.0F;
    i++;
  }

  DrawNodeCreateSandbox(ec, space, activeTemplate, activeNode);
}