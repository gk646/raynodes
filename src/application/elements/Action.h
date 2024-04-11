#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_

#include <cstdint>
#include <string>
#include <utility>

enum ActionType : uint8_t {
  MOVE,
  MASS_MOVE,
  TEXT_EDIT,
};

struct Action {
  ActionType type;
  explicit Action(ActionType type) : type(type){};
  virtual ~Action() = default;
  virtual void undo() = 0;
  virtual void redo() = 0;
  virtual const char* toString() {
    switch (type) {
      case MOVE:
        return "Node moved";
      case MASS_MOVE:
        return "Nodes moved";
      case TEXT_EDIT:
        return "Text edit";
    }
  };
};

struct TextAction final : public Action {
  std::string& targetText;  // Reference to the text being modified
  std::string beforeState;  // State of the text before the modification
  std::string afterState;   // State of the text after the modification
  TextAction(std::string& target, std::string before)
      : Action(TEXT_EDIT), targetText(target), beforeState(std::move(before)) {}
  inline void setAfter(std::string after) { afterState = std::move(after); }
  void undo() final;
  void redo() final;
};

#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
