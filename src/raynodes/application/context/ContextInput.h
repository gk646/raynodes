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

#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_
struct Input {
  bool mouseConsumed = false;
  bool keyboardConsumed = false;
  //Input wrappers
  [[nodiscard]] bool isMouseButtonPressed(const int button) const {
    return !mouseConsumed && IsMouseButtonPressed(button);
  }
  [[nodiscard]] bool isMouseButtonReleased(const int button) const {
    return IsMouseButtonReleased(button);  // Dont lock release
  }
  [[nodiscard]] bool isMouseButtonDown(const int button) const {
    return !mouseConsumed && IsMouseButtonDown(button);
  }
  [[nodiscard]] bool isKeyPressed(const int key) const { return !keyboardConsumed && IsKeyPressed(key); }
  [[nodiscard]] bool isKeyPressedRepeat(const int key) const { return !keyboardConsumed && IsKeyPressedRepeat(key); }
  [[nodiscard]] bool isKeyReleased(const int key) const { return !keyboardConsumed && IsKeyReleased(key); }
  [[nodiscard]] bool isKeyDown(const int key) const { return !keyboardConsumed && IsKeyDown(key); }
  void consumeMouse() { mouseConsumed = true; }
  void consumeKeyboard() { keyboardConsumed = true; }
  void reset() {
    mouseConsumed = false;
    keyboardConsumed = false;
  }
};
#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_