#ifndef RAYNODES_SRC_NODEEDITOR_H_
#define RAYNODES_SRC_NODEEDITOR_H_

#include <vector>

struct Node;
struct EditorContext;
class NodeEditor {
  inline static EditorContext* context;

 public:
  NodeEditor();
  bool start();
  int run();
  inline static EditorContext* getContext(){return context;};

 protected:
  virtual void onStart() {}
  virtual void onEnd() {}
  virtual void onFrame() {}
  virtual void drawFront() {}

 private:
  void controls();
  void saveNodes();
  void loadNodes();
  void drawBackGround();
  void drawForeGround();
  void drawContent();
};

#endif  //RAYNODES_SRC_NODEEDITOR_H_
