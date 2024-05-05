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

#include "shared/fwd.h"

#include <cxstructs/StackVector.h>

#include "component/Component.h"

#pragma warning(push)
#pragma warning(disable : 4100)  // unreferenced formal parameter

// ==============================
// NODE INTERFACE
// ==============================

// IMPORTANT: This is OPTIONAL. The base node handles all tasks automatically.
// This interface only allows to define additional actions, NOT redefine core functionality
// .....................................................................
// The node interface is for advanced use cases where inter-component data transfer is needed
// It allows to orchestrate components and add custom update and draw steps
// This can be due to serveral reasons:
//    - Having complex operations that need many inputs
//    - Having reached the input or output limit of components
//    - If you want to merge functinality to a single node
// .....................................................................

struct Node {
  cxstructs::StackVector<Component*, COMPONENTS_PER_NODE, int8_t> components;  // Fixed size
  InputPin nodeIn{NODE};       // Allows to signal connections to a node for any reason
  OutputPin nodeOut{NODE};     // Allows to signal connections to a node for any reason
  const char* name = nullptr;  //Unique allocated name
  float x, y;                  // Position
  float width, height;         // Dimensions
  uint8_t r, g, b, a;          // Header colour
  NodeID id;                   // Unqiue node ID
  uint16_t contentHeight = 0;  //Current height of the content
  bool isHovered = false;
  bool isDragged = false;

  explicit Node(const NodeTemplate& nt);
  Node(const Node& n, NodeID id);
  virtual ~Node();

  // Internal functions
  static void Update(EditorContext& ec, Node& n);
  static void Draw(EditorContext& ec, Node& n);
  static void SaveState(FILE* file, Node& n);
  static void LoadState(FILE* file, Node& n);

  //-----------CORE-----------//
  [[nodiscard]] virtual Node* clone(NodeID nid);
  // Guaranteed to be called each tick and AFTER all components have been updated
  virtual void update(EditorContext& ec) {}
  // Called only when the node bounds are (partially) within the screen
  virtual void draw(EditorContext& ec) {}
  // Called at save time - allows you to save arbitrary, unrelated additional state data
  virtual void saveState(FILE* file) {}
  // Called at load time - should be the exact same statements and order as in saveState();
  virtual void loadState(FILE* file) {}

  //-----------LIFE CYCLE-----------//
  // Called once when the node is created
  virtual void onCreation(EditorContext& ec) {}

  //-----------COMPONENTS-----------//
  Component* getComponent(const char* name);
  [[nodiscard]] int getComponentIndex(Component& c) const {
    for (int i = 0; i < components.size(); i++) {
      if (components[static_cast<int8_t>(i)] == &c) return i;
    }
    return -1;
  }
  void addComponent(Component* comp);

  //Helpers
  [[nodiscard]] Rectangle getExtendedBounds(float ext) const;
  [[nodiscard]] Rectangle getBounds() const;
  Color getColor() const;
};

struct MyNode : Node {
  MyNode(const NodeTemplate& nt) : Node(nt) {}
};

#pragma warning(pop)

#endif  //RAYNODES_SRC_NODES_NODES_H_