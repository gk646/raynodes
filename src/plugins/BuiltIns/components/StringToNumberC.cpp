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

#include "StringToNumberC.h"

void StringToNumberC::draw(EditorContext& ec, Node& parent) {

}

void StringToNumberC::update(EditorContext& ec, Node& parent) {
  auto inData = inputs[0].getData<PinType::STRING>();

  outputs[0].setData<PinType::FLOAT>(inData ? std::atof(inData) : 0.0);
  outputs[1].setData<PinType::INTEGER>(inData ? std::atoi(inData) : 0L);
}

void StringToNumberC::onCreate(EditorContext& ec, Node& parent) {
  addPinInput(PinType::STRING);

  addPinOutput(PinType::FLOAT);
  addPinOutput(PinType::INTEGER);
}

void StringToNumberC::save(FILE* file) {
  /*Not needed*/
}

void StringToNumberC::load(FILE* file) {
  /*Not needed*/
}