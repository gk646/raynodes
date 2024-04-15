#ifndef RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_
#define RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_

#include "component/Component.h"

struct StringToNumberC final : Component {
  explicit StringToNumberC(const char* name) : Component(name, STRING_TO_NUMBER, 50, 20) {}
  Component* clone() override { return new StringToNumberC(*this); };
  void draw(EditorContext& ec, Node& parent) override;
  void update(EditorContext& ec, Node& parent) override;
  void save(FILE* file) override;
  void load(FILE* file) override;

  void onCreate(EditorContext &ec, Node &parent) override;
};

#endif  //RAYNODES_SRC_COMPONENT_COMPONENTS_STRINGTONUMBERC_H_