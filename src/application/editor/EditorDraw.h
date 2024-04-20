#ifndef RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_
#define RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_

namespace Editor {
inline void DrawNodes(EditorContext& ec) {
  //TODO dont draw too small text or nodes
  auto& nodes = ec.core.nodes;
  const auto topLeft = GetScreenToWorld2D({0, 0}, ec.display.camera);
  const auto bottomRight = GetScreenToWorld2D(ec.display.screenSize, ec.display.camera);

  const Rectangle cameraBounds = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y};

  for (const auto n : nodes) {
    if (CheckCollisionRecs(cameraBounds, n->getBounds())) {
      n->draw(ec);
    }
  }
}
}  // namespace Editor
#endif  //RAYNODES_SRC_EDITOR_ELEMENTS_EDITORDISPLAY_H_