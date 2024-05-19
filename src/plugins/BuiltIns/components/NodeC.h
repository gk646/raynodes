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

#ifndef NODEC_H
#define NODEC_H

#include "component/Component.h"
#include "application/EditorContext.h"

template <ComponentStyle style = IN_AND_OUT>
struct NodeC final : Component {
  explicit NodeC(const ComponentTemplate ct) : Component(ct, 75, 20) {}
  Component* clone() override { return new NodeC(*this); }

  void draw(EditorContext& /**/, Node& /**/) override {
    const auto bounds = getBounds();
    DrawRectangleRec(bounds, UI::COLORS[UI_MEDIUM]);
  }

  void update(EditorContext& /**/, Node& /**/) override {}

  void onCreate(EditorContext& ec, Node& /**/) override {
    if constexpr (style == IN_AND_OUT || style == INPUT_ONLY) {
      addPinInput(NODE);
    } else if constexpr (style == IN_AND_OUT || style == OUTPUT_ONLY) {
      addPinOutput(NODE);
    }
  }
};

#endif  //NODEC_H