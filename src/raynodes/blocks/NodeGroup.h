#ifndef GROUP_H
#define GROUP_H

#include "shared/fwd.h"

#include <vector>
#include <unordered_map>

#include <raylib.h>
#include <cxstructs/StackVector.h>

struct StandalonePin {
  Node* node;
  Component* component;
  Pin* pin;
};

struct EXPORT NodeGroup {
  bool isRenaming = false;
  bool isHovered = false;
  bool isDragged = false;
  bool expanded = true;                 // Current status of the group
  Uints8 foldedDims;                    // So we can avoid measuring the name each tick
  Vector2 pos;                          // The position both unfolded and folded
  Vector2 dims;                         // The dimensions of the unfolded group - folded is deterministic
  const char* name;                     // Allocated pointer
  std::vector<Node*> nodes;             // Nodes that make up this group
  std::vector<StandalonePin> usedPins;  // Used in or out pins

  NodeGroup(EditorContext& ec, const char* name, std::unordered_map<NodeID, Node*> selectedNodes);
  NodeGroup(EditorContext& ec, const char* name);
  NodeGroup(NodeGroup&& other) noexcept
      : isHovered(other.isHovered), isDragged(other.isDragged), expanded(other.expanded),
        foldedDims(other.foldedDims), pos(other.pos), dims(other.dims), name(other.name),
        nodes(std::move(other.nodes)), usedPins(std::move(other.usedPins)) {
    other.name = nullptr;
  }
  NodeGroup& operator=(NodeGroup&& other) noexcept {
    if (this == &other) return *this;
    isHovered = other.isHovered;
    isDragged = other.isDragged;
    expanded = other.expanded;
    foldedDims = other.foldedDims;
    pos = other.pos;
    dims = other.dims;
    name = other.name;
    nodes = std::move(other.nodes);
    usedPins = std::move(other.usedPins);
    other.name = nullptr;
    return *this;
  }
  ~NodeGroup();
  bool operator==(const NodeGroup& other) const { return this == &other; }

  void draw(EditorContext& ec);
  void update(EditorContext& ec);
  [[nodiscard]] Rectangle getBounds() const;

  NodeGroup clone(Vector2 pos);

  void removeNode(EditorContext& ec, Node& node);
  void addNode(EditorContext& ec, Node& node);

  void onNodeMoved(EditorContext& ec, Node& node, Rectangle nBounds);
  void onConnectionAdded(EditorContext& ec, Node& node);

  static void InvokeMoved(EditorContext& ec, Node& node);
  static void InvokeDelete(EditorContext& ec, Node& node);
  static void InvokeConnection(EditorContext& ec, Node& node);

 private:
  void updateInternalState(const EditorContext& ec);
};

#endif  //GROUP_H