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

#include <raygui.h>

#include "HelpMenu.h"
#include "application/EditorContext.h"

void HelpMenu::drawContent(EditorContext& ec, const Rectangle& body) {
  constexpr float listWidth = 150.0F;
  const Rectangle listBounds = {body.x, body.y, listWidth, body.height};
  GuiListView(ec.display.getFullyScaled(listBounds), menuText, &scrollIndex, &activeIndex);

  Vector2 topLeft = {body.x + listWidth + UI::PAD, body.y + UI::PAD};

  if (activeIndex == 0) {  // Wiki
    if (UI::DrawButton(ec, topLeft, 150, 25, "Open the Wiki")) { OpenURL(Info::wikiLink); }
  } else if (activeIndex == 1) {  // Github
    if (UI::DrawButton(ec, topLeft, 150, 25, "Open the github page")) { OpenURL(Info::github); }
  } else if (activeIndex == 2) {  // About
    ec.string.formatText("%s %s", Info::applicationName, Info::getVersion(ec));
    UI::DrawText(ec, topLeft, ec.string.buffer);
    topLeft.y += UI::PAD;
    UI::DrawText(ec, topLeft, Info::about, UI::COLORS[UI_LIGHT], true);
  }
}