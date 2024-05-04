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

#ifndef RAYNODES_SRC_NODES_NODES_H_
#define RAYNODES_SRC_NODES_NODES_H_

#include <raylib.h>
#include <cxstructs/StackVector.h>

#include "shared/fwd.h"
#include "component/Component.h"

//TODO clean up - make ready for templates
//TODO remove raylib include

struct Node final {
  static constexpr float PADDING = 3;
  static constexpr float OFFSET_Y = 20;
  inline static Vector2 DRAG_OFFSET;
  cxstructs::StackVector<Component*, COMPONENTS_PER_NODE, int8_t> components{};  //Current limit
  const char* name = nullptr;                                                    //Unique allocated name
  Vector2 position{};
  Vector2 size;
  Color color;
  NodeID id;
  uint16_t contentHeight = 0;  //Current height of the content
  bool isHovered = false;
  bool isDragged = false;
  bool isInOutReversed = false;  //Not yet used

  explicit Node(const char* name) : name(name), size({70, 50}), color(BLACK) {}
  Node(const Node& n, const NodeID id)
      : name(n.name), position(n.position), size(n.size), color(n.color), id(id),
        isInOutReversed(n.isInOutReversed) {
    for (const auto c : n.components) {
      auto clone = c->clone();
      for (auto& in : clone->inputs) {
        in.connection = nullptr;  //Dont copy the connection ptr
        //TODO maybe copy connections but then fully -> create new connection objects
      }
      components.push_back(clone);
    }
  }
  ~Node();
  //Core
  void update(EditorContext& ec);
  void draw(EditorContext& ec);
  void saveState(FILE* file);
  void loadState(FILE* file);

  //Components
  Component* getComponent(const char* name);
  [[nodiscard]] int getComponentIndex(Component& c) const {
    for (int i = 0; i < components.size(); i++) {
      if (components[i] == &c) return i;
    }
    return -1;
  }
  void addComponent(Component* comp);

  Node* clone(const NodeID nid) { return new Node(*this, nid); }
  void exportToMQQS(std::ostream& out) {}

  //Helpers
  [[nodiscard]] Rectangle getExtendedBounds(const float ext) const {
    return {position.x - ext, position.y - ext, size.x + ext * 2, size.y + ext * 2};
  }
  [[nodiscard]] Rectangle getBounds() const { return {position.x, position.y, size.x, size.y}; }
};

#endif  //RAYNODES_SRC_NODES_NODES_H_