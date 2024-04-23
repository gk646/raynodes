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

enum class NodeType : uint8_t { HEADER, MATH, DISPLAY, STRING_CONVERSION, TOTAL_SIZE };

//TODO clean up - make ready for templates
struct Node {
  static constexpr float PADDING = 3;
  static constexpr float OFFSET_Y = 20;
  inline static Vector2 DRAG_OFFSET;

  cxstructs::StackVector<Component*, 7> components{};
  Vector2 position;
  Vector2 size;
  Color color;
  NodeID id;
  uint16_t contentHeight = 0;  //Current height of the content
  NodeType type;
  bool isHovered = false;
  bool isDragged = false;
  bool isInOutReversed = false;  //Not yet used

  Node(const NodeID id, const NodeType type, const Vector2 position = {0, 0}, const Color color = BLACK)
      : position(position), size({70, 50}), color(color), id(id), type(type) {}
  Node(const Node& n, const NodeID id)
      : position(n.position), size(n.size), color(n.color), id(id), type(n.type),
        isInOutReversed(n.isInOutReversed) {
    for (const auto c : n.components) {
      auto clone = c->clone();
      for (auto& in : clone->inputs) {
        in.connection = nullptr;  //Dont copy the connection ptr
      }
      components.push_back(clone);
    }
  }
  virtual ~Node();
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

  //Virtuals
  virtual Node* clone(const NodeID nid) { return new Node(*this, nid); }
  virtual void exportToMQQS(std::ostream& out) {}

  //Helpers
  [[nodiscard]] Rectangle getExtendedBounds(const float ext) const {
    return {position.x - ext, position.y - ext, size.x + ext * 2, size.y + ext * 2};
  }
  [[nodiscard]] Rectangle getBounds() const { return {position.x, position.y, size.x, size.y}; }
};

/* |-----------------------------------------------------|
 * |                DEFINE NODES HERE                    |
 * |-----------------------------------------------------|
 */

inline Node* CreateNode(const NodeID uid, const NodeType type, const Vector2 position) {
  switch (type) {
    case NodeType::HEADER:
      // return new HeaderNode(uid, position);
    case NodeType::MATH:
      // return new MathNode(uid, position);
    case NodeType::DISPLAY:
      // return new DisplayNode(uid, position);
    case NodeType::STRING_CONVERSION:
      break;
      //return new StringConversion(uid, position);
      /*
    case NodeType::GOTO:
      break;
    case NodeType::GOTO_PROXIMITY:
      break;
    case NodeType::KILL:
      break;
    case NodeType::SPAWN:
      break;
    case NodeType::SPEAK:
      break;
    case NodeType::SET_QUEST_ZONE:
      break;
    case NodeType::SCRIPTED_NODE:
      break;
    case NodeType::COLLECT:
      break;
    case NodeType::COMBAT_TRIGGER:
      break;
    case NodeType::REMOVE_ITEM:
      break;
    case NodeType::SET_QUEST_SHOWN:
      break;
    case NodeType::SWITCH_ALTERNATIVE:
      break;
    case NodeType::SET_NPC_POS:
      break;
    case NodeType::REQUIRE:
      break;
    case NodeType::FINISH_QUEST:
      break;
    case NodeType::DESPAWN_NPC:
      break;
    case NodeType::WAIT:
      break;
    case NodeType::PROTECT:
      break;
    case NodeType::CHOICE_DIALOGUE:
      break;
    case NodeType::ESCORT:
      break;
    case NodeType::MIX:
      break;
    case NodeType::NPC_MOVE:
      break;
    case NodeType::NPC_SAY:
      break;
    case NodeType::NPC_SAY_PROXIMITY:
      break;
    case NodeType::CHOICE_DIALOGUE_SIMPLE:
      break;
    case NodeType::TILE_ACTION:
      break;
    case NodeType::PLAYER_THOUGHT:
      break;
    case NodeType::OPTIONAL_POSITION:
      break;
    case NodeType::SKIP:
      break;
    case NodeType::TOTAL_SIZE:
      break;
       */
  }
  return nullptr;
}

inline const char* NodeToString(NodeType type) {
  switch (type) {
    case NodeType::HEADER:
      return "HEADER";
    case NodeType::MATH:
      return "Math Operation";
    case NodeType::DISPLAY:
      return "Display";
    case NodeType::STRING_CONVERSION:
      return "String Conversion";
      /*
    case NodeType::GOTO:
      return "GOTO";
    case NodeType::GOTO_PROXIMITY:
      return "GOTO_PROXIMITY";
    case NodeType::KILL:
      return "KILL";
    case NodeType::SPAWN:
      return "SPAWN";
    case NodeType::SPEAK:
      return "SPEAK";
    case NodeType::SET_QUEST_ZONE:
      return "SET_QUEST_ZONE";
    case NodeType::SCRIPTED_NODE:
      return "SCRIPTED_NODE";
    case NodeType::COLLECT:
      return "COLLECT";
    case NodeType::COMBAT_TRIGGER:
      return "COMBAT_TRIGGER";
    case NodeType::REMOVE_ITEM:
      return "REMOVE_ITEM";
    case NodeType::SET_QUEST_SHOWN:
      return "SET_QUEST_SHOWN";
    case NodeType::SWITCH_ALTERNATIVE:
      return "SWITCH_ALTERNATIVE";
    case NodeType::SET_NPC_POS:
      return "SET_NPC_POS";
    case NodeType::REQUIRE:
      return "REQUIRE";
    case NodeType::FINISH_QUEST:
      return "FINISH_QUEST";
    case NodeType::DESPAWN_NPC:
      return "DESPAWN_NPC";
    case NodeType::WAIT:
      return "WAIT";
    case NodeType::PROTECT:
      return "PROTECT";
    case NodeType::CHOICE_DIALOGUE:
      return "CHOICE_DIALOGUE";
    case NodeType::ESCORT:
      return "ESCORT";
    case NodeType::MIX:
      return "MIX";
    case NodeType::NPC_MOVE:
      return "NPC_MOVE";
    case NodeType::NPC_SAY:
      return "NPC_SAY";
    case NodeType::NPC_SAY_PROXIMITY:
      return "NPC_SAY_PROXIMITY";
    case NodeType::CHOICE_DIALOGUE_SIMPLE:
      return "CHOICE_DIALOGUE_SIMPLE";
    case NodeType::TILE_ACTION:
      return "TILE_ACTION";
    case NodeType::PLAYER_THOUGHT:
      return "PLAYER_THOUGHT";
    case NodeType::OPTIONAL_POSITION:
      return "OPTIONAL_POSITION";
    case NodeType::SKIP:
      return "SKIP";
    case NodeType::TOTAL_SIZE:
      return "TOTAL_SIZE";
       */

    default:
      return "Unknown";
  }
}

#endif  //RAYNODES_SRC_NODES_NODES_H_