#ifndef GROUP_H
#define GROUP_H

#include "shared/fwd.h"

#include <vector>
#include <unordered_map>

#include <raylib.h>
#include <cxstructs/StackVector.h>

#define MAX_INPUTS 6
#define MAX_OUTPUTS 6

struct EXPORT NodeGroup {
  bool isDragged = false;
  bool expanded = true;      // Current status of the group
  Uints8 unfoldedDims;       // So we can avoid measuring the name each tick
  Vector2 pos;               // The position both unfolded and folded
  Vector2 dims;              // The dimensions of the unfolded group - folded is deterministic
  const char* name;          // Allocated pointer
  std::vector<Node*> nodes;  // Nodes that make up this group

  NodeGroup(EditorContext& ec, const char* name, std::unordered_map<NodeID, Node*> selectedNodes);
  ~NodeGroup();
  NodeGroup clone(Vector2 pos);
  void draw(EditorContext& ec);
  void update(EditorContext& ec);
  [[nodiscard]] Rectangle getBounds() const;
};

#endif  //GROUP_H