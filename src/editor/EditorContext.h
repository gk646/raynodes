#ifndef RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
#define RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_

#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <nodes/Nodes.h>

enum class NodeType : uint8_t;

struct Node;
struct EditorContext {
  Font editorFont;
  NodeID UID = NodeID(1);
  Camera2D camera = {0};
  std::vector<Node*> nodes;
  std::unordered_map<NodeID, Node*> nodeMap;
  Vector2 dragStart = {0.0f, 0.0f};
  bool isDragging = false;
  bool showContextMenu = false;
  Vector2 contextMenuPos = {};

  EditorContext();

  //-----------NODES-----------//
  void createNode(NodeType type, Vector2 pos);
  void deleteNode(NodeID id);
  Node* getNode(NodeID id);

  //-----------UTIL-----------//
  static float getFontSize() { return 16.0F; };
  [[nodiscard]] inline Font getFont() const { return editorFont; }
};

#endif  //RAYNODES_SRC_EDITOR_EDITORCONTEXT_H_
