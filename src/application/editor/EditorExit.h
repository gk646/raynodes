#ifndef RAYNODES_SRC_APPLICATION_EDITOR_EDITOREXIT_H_
#define RAYNODES_SRC_APPLICATION_EDITOR_EDITOREXIT_H_

#include <cxstructs/Constraint.h>

namespace Editor {

inline int Exit(EditorContext& ec) {
  cxstructs::Constraint<true> c;

  c + ec.persist.saveToFile(ec);
  ec.core.logicThreadRunning = false;

  CloseWindow();

  return c.exitcode();
}
}  // namespace Editor
#endif  //RAYNODES_SRC_APPLICATION_EDITOR_EDITOREXIT_H_
