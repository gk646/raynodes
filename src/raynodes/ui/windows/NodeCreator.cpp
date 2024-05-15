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
#include "ui/elements/ListSearchMenu.h"
#include "ui/elements/TextPopup.h"

NodeCreator::NodeCreator(const Rectangle& r, const char* headerText) : Window(r, NODE_CREATOR, headerText) {
  searchField.growAutomatic = false;
  componentSearchField.growAutomatic = false;
  popupField.growAutomatic = false;
}

void NodeCreator::drawContent(EditorContext& ec, const Rectangle& body) {
  constexpr auto listWidth = 200.0F;

  // Draw search field
  drawSearchField(ec, body, listWidth);

  // Draw border
  const auto borderBounds = Rectangle{body.x, body.y, listWidth, body.height};
  UI::DrawRect(ec, borderBounds, 2, UI::COLORS[UI_DARK], BLANK);

  // Draw Create Button
  const auto listBounds = Rectangle{body.x + listWidth, body.y, 50, 20};
  if (UI::DrawButton(ec, listBounds, "#227#Add")) { showNamePopup = true; }

  NodeTemplate* activeTemplate = nullptr;
  if (searchField.hasUpdate()) { stringSort(ec.templates.userDefinedNodes, searchField.buffer, sortBuffer); }

  Rectangle entry = {body.x, body.y + UI::PAD, listWidth, 45};
  drawCreatedNodeList(ec, entry, activeTemplate);

  if (showNamePopup) {
    // Name validation func
    const auto vFunc = [](EditorContext& ec, const char* str) -> const char* {
      const int len = cxstructs::str_len(str);
      if (len > PLG_MAX_NAME_LEN) return "Name is too long!";
      if (len == 0) return "Name is too short";
      for (const auto& name : ec.templates.userDefinedNodes | std::ranges::views::keys) {
        if (strcmp(name, str) == 0) return "Name already exists!";
      }
      return nullptr;
    };

    auto popUp = UI::GetSubRect(body);
    constexpr auto* header = "Choose the node name";
    const auto* name = TextPopup::Draw(ec, popUp, popupField, vFunc, header);

    if (name == nullptr) [[likely]] { return; }
    if (strcmp(name, UI::DUMMY_STRING) == 0) showNamePopup = false;
    else {
      NodeTemplate nTemplate;
      nTemplate.label = cxstructs::str_dup(name);  // Need an allocated copy
      const auto createFunc = [](const NodeTemplate& nt, const Vec2 pos, const NodeID id) {
        return new Node(nt, pos, id);
      };
      ec.templates.registerUserNode(ec, nTemplate, createFunc);
      searchField.buffer = name;
      popupField.buffer.clear();
      activeEntry = 0;
      showNamePopup = false;
      stringSort(ec.templates.userDefinedNodes, searchField.buffer, sortBuffer);
      ec.ui.contextMenu.addNode("User Created",  nTemplate.label);
      setNode(ec, nTemplate);
    }
  }
  const Rectangle space = {body.x + listWidth, body.y, body.width - listWidth, body.height};
  drawNodeCreateSandbox(ec, space, activeTemplate);
}

void NodeCreator::setNode(EditorContext& ec, const NodeTemplate& nTemplate) {
  delete activeNode;
  activeNode = ec.templates.createNode(ec, nTemplate.label, {0, 0}, NodeID(0));
}

void NodeCreator::drawSearchField(EditorContext& ec, const Rectangle& body, float width) {
  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) searchField.onFocusGain(ec.logic.mouse);
  searchField.bounds = ec.display.getFullyScaled({body.x, body.y + 1, width, 20});
  searchField.update(ec, ec.logic.mouse);
  searchField.draw("Search...");
}

void NodeCreator::drawCreatedNodeList(EditorContext& ec, Rectangle& entry, NodeTemplate*& activeTemplate) {
  int i = 0;
  for (const auto nInfo : sortBuffer) {
    const bool selected = i == activeEntry;
    const Vector2 mouse = GetMousePosition();
    const Rectangle entryBounds = ec.display.getFullyScaled(entry);

    const Color text = selected ? UI::COLORS[UI_MEDIUM] : UI::COLORS[UI_LIGHT];
    Color background = selected ? UI::Darken(UI::COLORS[UI_LIGHT], 25) : UI::COLORS[UI_MEDIUM];
    Color border = UI::COLORS[UI_DARK];

    // Handle mouse click
    if (CheckCollisionPointRec(mouse, entryBounds)) {
      background = selected ? UI::Lighten(UI::COLORS[UI_LIGHT]) : UI::Lighten(UI::COLORS[UI_MEDIUM]);
      border = UI::Darken(UI::COLORS[UI_DARK]);
      if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (activeEntry != i) { setNode(ec, nInfo->nTemplate); }
        activeEntry = i;
      }
    }

    if (selected) activeTemplate = &nInfo->nTemplate;

    // Draw the entry
    UI::DrawRect(ec, entry, 2, border, background);
    UI::DrawText(ec, {entry.x + UI::PAD / 2.0F, entry.y + UI::PAD / 2.0F}, nInfo->nTemplate.label, text, false);

    // Draw action buttons
    constexpr auto buttonSize = 18.0F;
    const auto buttonY = entry.y + (entry.height - buttonSize) / 2.0F;
    const auto edit = Vector2{entry.x + entry.width - buttonSize * 3, buttonY};
    const auto remove = Vector2{entry.x + entry.width - buttonSize * 1.8F, buttonY};

    if (UI::DrawButton(ec, edit, buttonSize, buttonSize, "#022#")) { showRenameField = true; }
    if (UI::DrawButton(ec, remove, buttonSize, buttonSize, "#143#")) {
      if (ec.templates.userDefinedNodes.contains(nInfo->nTemplate.label)) {
        auto& eraseInfo = ec.templates.userDefinedNodes[nInfo->nTemplate.label];
        ec.ui.contextMenu.removeNode("User Created", eraseInfo.nTemplate.label);
        for (auto& [label, component] : eraseInfo.nTemplate.components) {
          delete component;
          delete label;
        }
        ec.templates.userDefinedNodes.erase(nInfo->nTemplate.label);
        delete eraseInfo.nTemplate.label;
        stringSort(ec.templates.userDefinedNodes, searchField.buffer, sortBuffer);
      }
    }

    entry.y += 45.0F;
    i++;
  }
}

void NodeCreator::drawNodeCreateSandbox(EditorContext& ec, Rectangle space, NodeTemplate* nTemplate) {
  if (nTemplate == nullptr || activeNode == nullptr) return;
  // TODO possibly draw the grid aswell / should look like the node editor
  // Draw BackGround
  // DrawRectangleRec(ec.display.getFullyScaled(space), UI::COLORS[N_BACK_GROUND]);
  const auto nodePos = ec.display.getFullyScaled(Vector2{space.x + UI::PAD * 5, space.y + UI::PAD * 3});

  activeNode->x = nodePos.x;
  activeNode->y = nodePos.y;

  Node::Update(ec, *activeNode);

  // Min dimensions
  activeNode->width = std::max(activeNode->width, 200.0F);
  activeNode->height = std::max(activeNode->height, 50.0F);

  Node::Draw(ec, *activeNode);

  if (UI::DrawButton(ec, {space.x + UI::PAD * 2, space.y + UI::PAD * 3}, 25, 25, "#112#")) {
    showComponentSearch = true;
  }

  if (showComponentSearch) {
    const auto bound = Vector2{space.x + UI::PAD * 2, space.y + UI::PAD * 3};
    const auto res = ListSearchMenu::Draw(ec, bound, componentSearchField, ec.templates.componentFactory);
    if (res == nullptr) return;
    if (strcmp(res, UI::DUMMY_STRING) == 0) {
      showComponentSearch = false;
    } else {
      showComponentSearch = false;
      for (auto& [label, component] : nTemplate->components) {
        if (component == nullptr) {
          label = cxstructs::str_dup(componentName.buffer.c_str());
          component = cxstructs::str_dup(res);
          setNode(ec, *nTemplate);
          break;
        }
      }
    }
  }

  if (ec.input.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) { componentName.onFocusGain(ec.logic.mouse); }
  componentName.bounds = ec.display.getFullyScaled({space.x + UI::PAD, space.y + UI::PAD, 150, 20});
  componentName.draw("ComponentName...");
  componentName.update(ec, ec.logic.mouse);

  if (UI::DrawButton(ec, {space.x + UI::PAD * 15, space.y + UI::PAD * 3}, 100, 25, "#128#Remove last")) {
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
  const int size = sortedNodes.size();
  if (size == 0) return;
  for (uint8_t i = 0; i < size - 1; ++i) {
    uint8_t minIndex = i;
    for (uint8_t j = i + 1; j < size; ++j) {
      if (cxstructs::str_sort_levenshtein_prefix<PLG_MAX_NAME_LEN>(sortedNodes[j]->nTemplate.label, searchCStr)
          < cxstructs::str_sort_levenshtein_prefix<PLG_MAX_NAME_LEN>(sortedNodes[minIndex]->nTemplate.label,
                                                                     searchCStr)) {
        minIndex = j;
      }
    }
    if (minIndex != i) { std::swap(sortedNodes[i], sortedNodes[minIndex]); }
  }
}