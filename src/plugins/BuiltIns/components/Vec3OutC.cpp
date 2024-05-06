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

#include "Vec3OutC.h"

#include <cxutil/cxio.h>

#include "application/EditorContext.h"

void Vec3OutC::draw(EditorContext& ec, Node& /**/) {
  auto bounds = getBounds();

  float startX = bounds.x;
  for (auto& f : textFields) {
    f.bounds.x = startX;
    f.bounds.y = bounds.y;
    f.draw();
    startX += 65.0F;
  }
}

void Vec3OutC::update(EditorContext& ec, Node& parent) {
  for (auto& f : textFields) {
    f.update(ec.logic.worldMouse);
  }

  Vec3 out{0.0F, 0.0F, 0.0F};

  out.x = cxstructs::str_parse_float(textFields[0].buffer.c_str());
  out.y = cxstructs::str_parse_float(textFields[1].buffer.c_str());
  out.z = cxstructs::str_parse_float(textFields[2].buffer.c_str());
  outputs[0].setData<VECTOR_3>(out);
}

void Vec3OutC::onFocusGain(EditorContext& ec) {
  for (auto& f : textFields) {
    f.onFocusGain(ec.logic.worldMouse);
  }
}

void Vec3OutC::onFocusLoss(EditorContext& ec) {
  for (auto& f : textFields) {
    f.onFocusLoss();
  }
}

void Vec3OutC::onCreate(EditorContext& ec, Node& /**/) {
  internalLabel = false;  //We don't want to draw our label
  for (auto& f : textFields) {
    f.font = &ec.display.editorFont;
    f.fs = ec.display.fontSize;
    f.minWidth = 60;
    f.bounds.width = 60;
    f.bounds.height = 20;
  }

  addPinOutput(VECTOR_3);
}

void Vec3OutC::save(FILE* file) {
  for (auto& f : textFields) {
    cxstructs::io_save(file, f.buffer.c_str());
  }
}

void Vec3OutC::load(FILE* file) {
  for (auto& f : textFields) {
    cxstructs::io_load(file, f.buffer);
  }
}