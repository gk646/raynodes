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

#ifndef VECTOR3OUTPUTC_H
#define VECTOR3OUTPUTC_H

#include "component/Component.h"
#include "ui/TextField.h"

class Vec3OutC final : public Component {
  TextInputField textFields[3]{};

 public:
  explicit Vec3OutC(const ComponentTemplate ct) : Component(ct, 200, 20) {}
  Component* clone() override { return new Vec3OutC(*this); }
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext&, Node& parent) override;
  void load(FILE* file) override;
  void save(FILE* file) override;
  void onFocusGain(EditorContext&) override;
  void onFocusLoss(EditorContext&) override;
  void onCreate(EditorContext& ec, Node& parent) override;
};

#endif  //VECTOR3OUTPUTC_H