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

#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "shared/fwd.h"

enum ActionType : uint8_t {
  NEW_CANVAS_ACTION,
  MOVE_NODE,
  TEXT_EDIT,
  DELETE_NODE,
  CREATE_NODE,
  CONNECTION_DELETED
};

//Action was already performed when created
//-> Current state includes this performed action
//Undo means reversing this action
//Redo means performing this action again
struct Action {
  ActionType type;
  explicit Action(const ActionType type) : type(type) {}
  virtual ~Action() noexcept = default;
  virtual void undo(EditorContext& ec) = 0;
  virtual void redo(EditorContext& ec) = 0;

  //Allows for custom strings with context specific information
  [[nodiscard]] virtual const char* toString() const {
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
      case CONNECTION_DELETED:
        return "Connection(s) deleted";
    }
    return "Unknown";
  }
};

struct NewCanvasAction final : Action {
  NewCanvasAction() : Action(NEW_CANVAS_ACTION) {}
  void undo(EditorContext& /**/) override {}
  void redo(EditorContext& /**/) override {}
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
  std::vector<Connection*> deletedConnections;
  explicit NodeDeleteAction(EditorContext& ec, const std::unordered_map<NodeID, Node*>& selectedNodes);
  ~NodeDeleteAction() noexcept override;
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;

 private:
  bool removeNodes = true;
};

struct NodeCreateAction final : Action {
  std::vector<Node*> createdNodes;
  explicit NodeCreateAction(int size);
  ~NodeCreateAction() noexcept override;
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;

 private:
  bool removeNodes = false;
};

//Saves the move delta
struct NodeMovedAction final : Action {
  std::vector<std::pair<NodeID, Vector2>> movedNodes;
  explicit NodeMovedAction(int size);
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;
  float calculateDeltas( EditorContext& ec);
};

struct ConnectionDeleteAction final : Action {
  std::vector<Connection*> deletedConnections;
  explicit ConnectionDeleteAction(int size);
  ~ConnectionDeleteAction() noexcept override;
  void undo(EditorContext& ec) override;
  void redo(EditorContext& ec) override;
  float calculateDeltas(EditorContext& ec);

 private:
  bool removeNodes = true;
};

#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORACTION_H_