#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_
struct Input {
  bool mouseConsumed = false;
  bool keyboardConsumed = false;
  //Input wrappers
  [[nodiscard]] bool isMouseButtonPressed(int button) const {
    return !mouseConsumed && IsMouseButtonPressed(button);
  }
  [[nodiscard]] bool isMouseButtonReleased(int button) const {
    return !mouseConsumed && IsMouseButtonReleased(button);
  }
  [[nodiscard]] bool isMouseButtonDown(int button) const {
    return !mouseConsumed && IsMouseButtonDown(button);
  }
  [[nodiscard]] bool isKeyPressed(int key) const { return !keyboardConsumed && IsKeyPressed(key); }
  [[nodiscard]] bool isKeyPressedRepeat(int key) const {
    return !keyboardConsumed && IsKeyPressedRepeat(key);
  }
  [[nodiscard]] bool isKeyReleased(int key) const {
    return !keyboardConsumed && IsKeyReleased(key);
  }
  [[nodiscard]] bool isKeyDown(int key) const { return !keyboardConsumed && IsKeyDown(key); }
  void consumeMouse() { mouseConsumed = true; }
  void consumeKeyboard() { keyboardConsumed = true; }
  void reset() {
    mouseConsumed = false;
    keyboardConsumed = false;
  }
};
#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTINPUT_H_
