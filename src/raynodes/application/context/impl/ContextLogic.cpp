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

#include <ranges>

#include "application/EditorContext.h"
#include "application/elements/Action.h"

namespace {
void AssignConnection(EditorContext& ec, Node& fromNode, Component* from, OutputPin& out, Node& toNode,
                      Component* to, InputPin& in) {
  if (!out.isConnectable(ec, in)) return;
  auto* action = new ConnectionCreateAction(2);

  const auto conn = new Connection(fromNode, from, out, toNode, to, in);

  action->createdConnections.push_back(conn);
  ec.core.addEditorAction(ec, action);

  printf("Connection assigned \n");

  ec.core.addConnection(conn);  //Internally opens it

  //Call the event functions
  if (from) from->onConnectionAdded(ec, *conn);
  if (to) to->onConnectionAdded(ec, *conn);
  if (fromNode.isInGroup) [[unlikely]] { NodeGroup::InvokeConnection(ec, fromNode); }
  if (toNode.isInGroup) [[unlikely]] { NodeGroup::InvokeConnection(ec, toNode); }
}

void FindDropPin(EditorContext& ec, Node& fromNode, Component* from, Pin* draggedPin) {
  const auto worldMouse = ec.logic.worldMouse;
  constexpr auto radius = Pin::PIN_SIZE / 2.0F;

  const bool isOutputPin = draggedPin->direction == OUTPUT;

  for (const auto n : ec.core.nodes) {
    //Extended Bound check on the node to skip iterating components
    const auto nodeBounds = n->getExtendedBounds(Pin::PIN_SIZE);

    if (CheckCollisionPointRec(worldMouse, nodeBounds)) [[unlikely]] {
      //Allow both types to connect to each other
      if (isOutputPin) {
        for (auto* to : n->components) {
          for (auto& in : to->inputs) {
            if (CheckCollisionPointCircle(worldMouse, {in.xPos, in.yPos}, radius)) {
              AssignConnection(ec, fromNode, from, *static_cast<OutputPin*>(draggedPin), *n, to, in);
              return;
            }
          }
        }
        // Node input
        if (CheckCollisionPointCircle(worldMouse, {n->x, n->nodeIn.yPos}, radius)) {
          AssignConnection(ec, fromNode, from, *static_cast<OutputPin*>(draggedPin), *n, nullptr, n->nodeIn);
          return;
        }
      } else {
        for (auto* to : n->components) {
          for (auto& out : to->outputs) {
            if (CheckCollisionPointCircle(worldMouse, {out.xPos, out.yPos}, radius)) {
              //Confusing assignment - orders are switched - target node is now output
              //Connection interface is still kept the same for clarity
              AssignConnection(ec, *n, to, out, fromNode, from, *static_cast<InputPin*>(draggedPin));
              return;
            }
          }
        }
        for (auto& out : n->outputs) {
          if (CheckCollisionPointCircle(worldMouse, {out.xPos, out.yPos}, radius)) {
            AssignConnection(ec, *n, nullptr, out, fromNode, from, *static_cast<InputPin*>(draggedPin));
            return;
          }
        }
      }
    }
  }

  // This comes at no cost in most cases
  for (const auto& g : ec.core.nodeGroups) {
    for (const auto [node, component, pin] : g.usedPins) {
      if (CheckCollisionPointCircle(worldMouse, {pin->xPos, pin->yPos}, radius)) {
        if (isOutputPin) {
          AssignConnection(ec, fromNode, from, *static_cast<OutputPin*>(draggedPin), *node, component,
                           *static_cast<InputPin*>(pin));
          return;
        }
        AssignConnection(ec, *node, component, *static_cast<OutputPin*>(pin), fromNode, from,
                         *static_cast<InputPin*>(draggedPin));
        return;
      }
    }
  }
}
}  // namespace

void Logic::handleDroppedPin(EditorContext& ec) {
  if (!isMakingConnection) return;

  //Cache with correct names
  auto& fromNode = *draggedPinNode;
  auto* from = draggedPinComponent;  // NULL when node-to-node

  //Reset regardless if a pin is found
  FindDropPin(ec, fromNode, from, draggedPin);

  //Correct order to give strong guarantee when "isMakingConnection" holds
  isMakingConnection = false;
  draggedPinNode = nullptr;
  draggedPin = nullptr;
  draggedPinComponent = nullptr;
  draggedPinPos = {};
}

void Logic::registerNodeContextActions(EditorContext& ec) {
  // Node Menu
  {
    ec.ui.nodeContextMenu.registerAction(
        "Remove connections",
        [](EditorContext& ec, Node& node) {
          // Cant fully test for connections unless we iterate connections
          // -> Outputs dont track connections!
          auto* action = new ConnectionDeleteAction(2);
          ec.core.removeConnectionsFromNode(node, action->deletedConnections);
          if (action->deletedConnections.empty()) delete action;
          else ec.core.addEditorAction(ec, action);
        },
        175);

    ec.ui.nodeContextMenu.registerAction(
        "Create node group",
        [](EditorContext& ec, Node& node) {
          // Check upfront if at least 1 free node exists
          for (const auto n : ec.core.selectedNodes | std::ranges::views::values) {
            if (!n->isInGroup) {
              ec.core.nodeGroups.emplace_back(ec, "Node Group", ec.core.selectedNodes);
              return;
            }
          }
        },
        109);

    // Quick Actions

    ec.ui.nodeContextMenu.registerQickAction(
        "Cut (Ctrl+X)",
        [](EditorContext& ec, Node& node) {
          ec.core.selectedNodes.insert({node.uID, &node});
          ec.core.cut(ec);
        },
        17);
    ec.ui.nodeContextMenu.registerQickAction(
        "Copy (Ctrl+C)",
        [](EditorContext& ec, Node& node) {
          ec.core.selectedNodes.insert({node.uID, &node});
          ec.core.copy(ec);
        },
        18);

    ec.ui.nodeContextMenu.registerQickAction(
        "Delete (Delete)",
        [](EditorContext& ec, Node& node) {
          ec.core.selectedNodes.insert({node.uID, &node});
          ec.core.erase(ec);
        },
        143);
  }

  // Node Group Menu
  {
    ec.ui.nodeGroupContextMenu.registerQickAction(
        "Refresh",
        [](EditorContext& ec, NodeGroup& ng) {
            ng.onConnectionAdded(ec,*ng.nodes[0]);
        },
        222);

    ec.ui.nodeGroupContextMenu.registerAction(
        "Remove node group",
        [](EditorContext& ec, NodeGroup& ng) {
          for (const auto n : ng.nodes) {
            ng.removeNode(ec, *n);
          }
        },
        143);
  }

  // Canvas Menu
  {
    ec.ui.canvasContextMenu.registerQickAction(
        "Open node menu",
        [](EditorContext& ec, Vector2& _) {
          ec.ui.canvasContextMenu.hide();
          ec.logic.contextMenuPos = ec.logic.mouse;
          ec.ui.nodeCreateMenu.show();
        },
        227);

    ec.ui.canvasContextMenu.registerAction(
        "Add comment",
        [](EditorContext& ec, Vector2& _) {

        },
        30);

    ec.ui.canvasContextMenu.registerAction(
        "Import nodes here", [](EditorContext& ec, Vector2& _) { ec.persist.importNodesFromProject(ec); }, 209);
  }
}