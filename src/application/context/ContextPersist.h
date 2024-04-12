#ifndef RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTPERSIST_H_
#define RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTPERSIST_H_

struct EditorContext;

struct Persist
{
  const char* openedFile;
  bool loadFromFile(EditorContext& ec) const;
  bool saveToFile(EditorContext& ec) const;
};

#endif  //RAYNODES_SRC_APPLICATION_CONTEXT_CONTEXTPERSIST_H_
