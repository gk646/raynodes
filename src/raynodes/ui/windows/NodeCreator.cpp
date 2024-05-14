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

NodeCreator::NodeCreator(const Rectangle& r, const char* headerText) : Window(r, NODE_CREATOR, headerText) {
  searchField.growAutomatic = false;
}

void NodeCreator::drawContent(EditorContext& ec, const Rectangle& body) {
  // Cache
  const auto mouse = ec.logic.mouse;

  // Draw Border
  const auto borderBounds = Rectangle{body.x, body.y, 150, body.height};
  UI::DrawRect(ec, borderBounds, 2, UI::COLORS[UI_DARK], BLANK);

  drawSearchField(ec, body);

  // Draw Create Button
  auto listBounds = Rectangle{body.x + UI::PAD * 7.0F, body.y, 150, 50};
  if (UI::DrawButton(ec, listBounds, "Create a custom node")) { showNamePopup = true; }

  if (showNamePopup) {
    const auto smallWindow = UI::GetCenteredWindowBounds<false>();
    auto name = UI::DrawTextPopUp(ec, smallWindow, "Choose a name for your node:", showNamePopup);
    if (name != nullptr) {
      NodeTemplate nTemplate;
      nTemplate.label = cxstructs::str_dup(name);  // Need a allocated copy
      const auto createFunc = [](const NodeTemplate& nt, Vec2 pos, NodeID id) {
        return new Node(nt, pos, id);
      };
      ec.templates.registerUserNode(ec, nTemplate, createFunc);
      searchField.buffer = name;
      activeEntry = 0;
      stringSort(ec.templates.userDefinedNodes, searchField.buffer, sortBuffer);
      setNode(ec, nTemplate);
    }
  }

  Rectangle entry = {body.x, body.y + UI::PAD, 150, 50};
  Rectangle space = {body.x + 150.0F, body.y, body.width - 150.0F, body.height};
  NodeTemplate* activeTemplate = nullptr;

  if (searchField.hasUpdate()) { stringSort(ec.templates.userDefinedNodes, searchField.buffer, sortBuffer); }

  int i = 0;
  for (auto& nInfo : sortBuffer) {
    const bool selected = i == activeEntry;
    const auto text = selected ? UI::COLORS[UI_MEDIUM] : UI::COLORS[UI_LIGHT];
    const auto background = selected ? UI::COLORS[UI_LIGHT] : UI::COLORS[UI_MEDIUM];
    const auto border = UI::COLORS[UI_DARK];

    if (selected) activeTemplate = &nInfo->nTemplate;

    UI::DrawRect(ec, entry, 2, border, background);
    UI::DrawText(ec, {entry.x, entry.y + UI::PAD}, nInfo->nTemplate.label, text, false);

    const auto entryBounds = ec.display.getFullyScaled(entry);
    if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, entryBounds)) {
      if (activeEntry != i) { setNode(ec, nInfo->nTemplate); }
      activeEntry = i;
    }
    entry.y += 50.0F;
    i++;
  }

  drawNodeCreateSandbox(ec, space, activeTemplate);
}

void NodeCreator::setNode(EditorContext& ec, const NodeTemplate& nTemplate) {
  delete activeNode;
  activeNode = ec.templates.createNode(ec, nTemplate.label, {0, 0}, NodeID(0));
}

void NodeCreator::drawSearchField(EditorContext& ec, const Rectangle& body) {
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) searchField.onFocusGain(ec.logic.mouse);
  searchField.bounds = ec.display.getFullyScaled({body.x + UI::PAD, body.y, 150, 20});
  searchField.update(ec.logic.mouse);
  searchField.draw();
}

void NodeCreator::drawNodeCreateSandbox(EditorContext& ec, Rectangle space, NodeTemplate* nTemplate) {
  if (nTemplate == nullptr || activeNode == nullptr) return;
  // TODO possibly draw the grid aswell / should look like the node editor
  // Draw BackGround
  // DrawRectangleRec(ec.display.getFullyScaled(space), UI::COLORS[N_BACK_GROUND]);
  auto nodePos = ec.display.getFullyScaled(Vector2{space.x + UI::PAD * 7, space.y + UI::PAD * 3});

  activeNode->x = nodePos.x;
  activeNode->y = nodePos.y;
  Node::Update(ec, *activeNode);
  Node::Draw(ec, *activeNode);

  if (UI::DrawButton(ec, {space.x + UI::PAD * 2, space.y + UI::PAD * 3}, 25, 25, "#112#")) {
    for (int i = 0; i < COMPS_PER_NODE; ++i) {
      if (nTemplate->components[i].component == nullptr) {
        nTemplate->components[i].label = cxstructs::str_dup("TextField");
        nTemplate->components[i].component = cxstructs::str_dup("BI_Text");
        setNode(ec, *nTemplate);
        break;
      }
    }
  }

  if (UI::DrawButton(ec, {space.x + UI::PAD * 12, space.y + UI::PAD * 3}, 25, 25, "#128#")) {
    for (int i = COMPS_PER_NODE - 1; i > -1; --i) {
      if (nTemplate->components[i].component != nullptr) {
        delete nTemplate->components[i].label;
        delete nTemplate->components[i].component;
        nTemplate->components[i].component = nullptr;
        nTemplate->components[i].label = nullptr;
        setNode(ec, *nTemplate);
        break;
      }
    }
  }
}

void NodeCreator::stringSort(auto& userCreatedTemplates, const std::string& searchText, auto& sortedNodes) {
  sortedNodes.clear();
  for (auto& value : userCreatedTemplates | std::ranges::views::values) {
    sortedNodes.push_back(&value);
  }

  const char* searchCStr = searchText.c_str();
  size_t size = sortedNodes.size();

  for (size_t i = 0; i < size - 1; ++i) {
    size_t minIndex = i;
    for (size_t j = i + 1; j < size; ++j) {
      if (cxstructs::str_sort_levenshtein_prefix<Plugin::MAX_NAME_LEN>(sortedNodes[j]->nTemplate.label, searchCStr)
          < cxstructs::str_sort_levenshtein_prefix<Plugin::MAX_NAME_LEN>(sortedNodes[minIndex]->nTemplate.label,
                                                                         searchCStr)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(sortedNodes[i], sortedNodes[minIndex]); }
  }
}