#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_

#include <cstdint>
#include <string>

enum ActionType : uint8_t {
  MOVE,
  MASS_MOVE,
  TEXT_EDIT,
};

struct Action {
  ActionType type;
  explicit Action(ActionType type) : type(type){};
  virtual void undo() = 0;
  virtual void redo() = 0;
};

struct TextAction final : public Action {
  std::string& targetText; // Reference to the text being modified
  std::string beforeState; // State of the text before the modification
  std::string afterState;  // State of the text after the modification
  TextAction(std::string& target, const std::string& before, const std::string& after)
      : Action(TEXT_EDIT), targetText(target), beforeState(before), afterState(after) {}
  void undo() final;
  void redo() final;
};

#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
