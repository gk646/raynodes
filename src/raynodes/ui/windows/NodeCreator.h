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

#ifndef NODECREATOR_H
#define NODECREATOR_H

#include <cxstructs/StackVector.h>

#include "ui/Window.h"
#include "ui/elements/TextField.h"

struct NodeCreator final : Window {
  cxstructs::StackVector<NodeInfo*, 50, uint8_t> sortBuffer;
  TextField searchField{150, 18};
  TextField componentSearchField{150, 18};
  TextField popupField{150, 18};
  Node* activeNode = nullptr;
  int activeEntry = 0;
  bool showNamePopup = false;
  bool showComponentSearch = false;

  NodeCreator(const Rectangle& r, const char* headerText);
  void drawContent(EditorContext& ec, const Rectangle& body) override;
  void onClose() override {
    showNamePopup = false;
    searchField.isFocused = false;
    componentSearchField.isFocused = false;
    popupField.isFocused = false;
    showComponentSearch = false;
  }

 private:
  void setNode(EditorContext& ec, const NodeTemplate& nTemplate);
  void drawSearchField(EditorContext& ec, const Rectangle& body);
  void drawCreatedNodeList(EditorContext& ec, Rectangle& entry, NodeTemplate*& nTemplate);
  void drawNodeCreateSandbox(EditorContext& ec, Rectangle space, NodeTemplate* nTemplate);
  static void stringSort(auto& userCreatedTemplates, const std::string& searchText, auto& sortedNodes);
};

#endif  //NODECREATOR_H