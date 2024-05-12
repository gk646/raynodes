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

#ifndef SEPARATEXYZ_H
#define SEPARATEXYZ_H

#include "component/Component.h"

struct SeparateXYZC final : Component {
  explicit SeparateXYZC(const ComponentTemplate ct) : Component(ct, 100, 20) {}
  Component* clone() override { return new SeparateXYZC(*this); }

  void draw(EditorContext& /**/, Node& /**/) override {}
  void update(EditorContext& ec, Node& parent) override {
    Vec3 outVec = inputs[0].getData<VECTOR_3>();

    outputs[0].setData<FLOAT>(outVec.x);
    outputs[1].setData<FLOAT>(outVec.y);
    outputs[2].setData<FLOAT>(outVec.z);
  }

  void onCreate(EditorContext& ec, Node& parent) override {
    addPinInput(VECTOR_3);

    addPinOutput(FLOAT);
    addPinOutput(FLOAT);
    addPinOutput(FLOAT);
  }
};

#endif  //SEPARATEXYZ_H