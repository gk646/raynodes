#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_

#include "component/Component.h"

struct DisplayC final : Component {
  explicit DisplayC(const char* name) : Component(name, MATH_OPERATION, 250, 20) {}
  Component* clone() override { return new DisplayC(*this); };
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext& ec, Node& parent) override;
  void save(FILE* file) override;
  void load(FILE* file) override;

  void onCreate(EditorContext& ec, Node& parent) override;
};
#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_DISPLAYC_H_