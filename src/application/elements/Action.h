#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_

#include <string>
#include <vector>

#include "shared/fwd.h"
#include "shared/Types.h"

enum ActionType : uint8_t { NEW_CANVAS_ACTION, MOVE_NODE, TEXT_EDIT, DELETE_NODE, CREATE_NODE };

//Action was already performed when created
//-> Current state includes this performed action
//Undo means reversing this action
//Redo means performing this action
struct Action {
  ActionType type;

  explicit Action(const ActionType type) : type(type) {}

  virtual ~Action() noexcept = default;
  virtual void undo(EditorContext& ec) = 0;
  virtual void redo(EditorContext& ec) = 0;

  //Allows for custom strings with context specific information
  virtual const char* toString() {
    switch (type) {
      case TEXT_EDIT:
        return "Text edit";
      case NEW_CANVAS_ACTION:
        return "New Canvas";
      case MOVE_NODE:
        return "Node(s) moved";
      case DELETE_NODE:
        return "Node(s) deleted";
      case CREATE_NODE:
        return "Node(s) created";
    }
    return "Unknown";
  }
};

struct NewCanvasAction final : public Action {
  NewCanvasAction() : Action(NEW_CANVAS_ACTION) {}

  void undo(EditorContext& ec) override {}
  void redo(EditorContext& ec) override {}
};

struct TextAction final : Action {
  std::string& targetText;  // Reference to the text being modified
  std::string beforeState;  // State of the text before the modification
  std::string afterState;   // State of the text after the modification

  TextAction(std::string& target, std::string before)
      : Action(TEXT_EDIT), targetText(target), beforeState(std::move(before)) {}

  void setAfter(std::string after) { afterState = std::move(after); }

  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;
};

struct NodeDeleteAction final : Action {
  std::vector<Node*> deletedNodes;
  explicit NodeDeleteAction(int size);
  ~NodeDeleteAction() noexcept override;
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;

 private:
  bool removeNodes = true;  //Complicated to understand /just hide it
};

struct NodeCreateAction final : Action {
  std::vector<Node*> createdNodes;
  explicit NodeCreateAction(int size);
  ~NodeCreateAction() noexcept override;
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;

 private:
  bool removeNodes = true;  //Complicated to understand /just hide it
};

//Saves the move delta
struct NodeMovedAction final : Action {
  std::vector<std::pair<NodeID, Vector2>> movedNodes;
  explicit NodeMovedAction(int size);
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;
  float calculateDeltas(EditorContext& ec);
};

#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
