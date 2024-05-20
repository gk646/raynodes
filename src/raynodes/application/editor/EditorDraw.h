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

#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_

namespace Editor {
inline void DrawNodes(EditorContext& ec) {
  const auto& nodes = ec.core.nodes;
  const auto topLeft = GetScreenToWorld2D({0, 0}, ec.display.camera);
  const auto bottomRight = GetScreenToWorld2D(ec.display.screenSize, ec.display.camera);

  const Rectangle cameraBounds = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y};

  for (const auto n : nodes) {
    if (CheckCollisionRecs(cameraBounds, n->getBounds())) { Node::Draw(ec, *n); }
  }
}
inline void DrawConnections(EditorContext& ec, bool isCTRLDown) {
  const auto& connections = ec.core.connections;
  const auto selectRect = ec.logic.selectRect;
  const bool delNodes = isCTRLDown && ec.input.isMouseButtonReleased(MOUSE_BUTTON_RIGHT);

  ConnectionDeleteAction* action = nullptr;
  if (delNodes) action = new ConnectionDeleteAction(2);

  for (const auto conn : connections) {
    const auto fromPos = conn->getFromPos();
    const auto toPos = conn->getToPos();
    DrawLineBezier(fromPos, toPos, 2, conn->getConnectionColor());

    if (delNodes && CheckCollisionBezierRect(fromPos, toPos, selectRect)) {
      action->deletedConnections.push_back(conn);
      ec.core.removeConnection(conn);
    }
  }
  if (delNodes) [[unlikely]] {
    if (action->deletedConnections.empty()) delete action;
    else ec.core.addEditorAction(ec, action);
  }
}
}  // namespace Editor
#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_