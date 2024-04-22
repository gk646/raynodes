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

#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_

#include "component/Component.h"

struct StringToNumberC final : Component {
  explicit StringToNumberC(const char* name) : Component(name, STRING_TO_NUMBER, 50, 20) {}
  Component* clone() override { return new StringToNumberC(*this); };
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext& ec, Node& parent) override;
  void save(FILE* file) override;
  void load(FILE* file) override;

  void onCreate(EditorContext &ec, Node &parent) override;
};

#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_