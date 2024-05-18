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

#include "application/EditorContext.h"

#include <ranges>
#include <unordered_set>
#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>
#include <tinyfiledialogs.h>

//TODO make save format that combines both saving and loading
//-> should be easy as its symmetric - function wrapper and boolean for loading-saving
// Errors that we should detect:
// - Non Unique Node labels when loading templates (DONE)
// - Symbol name is too long (component or node) (DONE)
// - Non Unique Component Labels when loading templates (per node)
// - Missing definitions for loaded node (not present in factory maps)
// - Missing definitions for loaded component (not present in factory maps)

using PersistFunc = bool (*)(EditorContext&, FILE*);

static constexpr int MAX_UNIQUE_LABELS = 512;
// One hot encoding the labels
struct ComponentIndices {
  char storage[PLG_MAX_NAME_LEN * MAX_UNIQUE_LABELS] = {};
  uint16_t count = 0;
  int add(const char* name, int index = -1) {
    if (name == nullptr || count >= MAX_UNIQUE_LABELS || get(name) != -1) return -1;
    int addIndex = index == -1 ? count : index;
    char* ptr = storage + addIndex * PLG_MAX_NAME_LEN;
    int added = 0;
    while (added < PLG_MAX_NAME_LEN && *(name + added) != '\0') {
      *(ptr + added) = *(name + added);
      added++;
    }
    count++;
    return count - 1;
  }
  int get(const char* name) const {
    if (name == nullptr) return -1;
    for (int i = 0; i < count; ++i) {
      if (strncmp(storage + i * PLG_MAX_NAME_LEN, name, PLG_MAX_NAME_LEN) == 0) { return i; }
    }
    return -1;
  }
  [[nodiscard]] const char* getName(int index) const {
    if (index == -1 || index > count || index >= MAX_UNIQUE_LABELS) return nullptr;
    return storage + index * PLG_MAX_NAME_LEN;
  }
  void reset() {
    std::memset(storage, 0, sizeof(storage));
    count = 0;
  }
};

// Static instance on the stack - this is only needed inside here
// Pure file size optimization the rest of the program doesnt know about
static ComponentIndices compIndices{};

//-----------PROJECT_FILES-----------//
using namespace cxstructs;  // using the namespace here
namespace {
bool IsValidConnection(int maxNodeID, int fromNode, int from, int out, int toNode, int to, int in) {
  const bool correctNode = fromNode >= 0 && fromNode < maxNodeID && toNode >= 0 && toNode < maxNodeID;
  const bool correctFromComponent = (from >= 0 && from < COMPS_PER_NODE) || from == -1;
  const bool correctToComponent = (to >= 0 && to < COMPS_PER_NODE) || to == -1;
  return correctNode && correctFromComponent && correctToComponent && out != -1 && in != -1;
}
void CreateNewConnection(EditorContext& ec, int fromID, int fromI, int outI, int toID, int toI, int inI) {
  const auto& nodeMap = ec.core.nodeMap;
  // We use int8_t as size_type to save space
  Node& fromNode = *nodeMap.at(static_cast<NodeID>(fromID));

  Component* from = nullptr;
  OutputPin* out;
  if (fromI != -1) {
    from = fromNode.components[static_cast<int8_t>(fromI)];
    out = &from->outputs[static_cast<int8_t>(outI)];
  } else {
    out = &fromNode.outputs[static_cast<int8_t>(outI)];
  }

  Node& toNode = *nodeMap.at(static_cast<NodeID>(toID));
  Component* to = nullptr;
  InputPin* in = nullptr;
  if (toI != -1) {
    to = toNode.components[static_cast<int8_t>(toI)];
    in = &to->inputs[static_cast<int8_t>(inI)];
  } else {
    in = &toNode.nodeIn;
  }

  ec.core.addConnection(new Connection(fromNode, from, *out, toNode, to, *in));
}
void SaveEditorData(FILE* file, EditorContext& ec) {
  io_save_section(file, "EditorData");
  io_save(file, static_cast<int>(ec.core.nodes.size()));        // Total nodes
  io_save(file, static_cast<int>(ec.core.connections.size()));  // Total connections
  io_save(file, ec.display.camera.target.x);
  io_save(file, ec.display.camera.target.y);
  io_save(file, ec.display.camera.zoom);
  io_save_newline(file);
}
void SaveTemplates(FILE* file, EditorContext& ec) {
  io_save_section(file, "Templates");

  // Only save unique nodes
  std::unordered_set<const char*, Fnv1aHash, StrEqual> uniqueNodes;
  for (const auto node : ec.core.nodes) {
    uniqueNodes.insert(node->name);
  }

  io_save(file, static_cast<int>(uniqueNodes.size()));
  io_save_newline(file);

  // Save func
  constexpr auto saveTemplate = [](FILE* file, const NodeInfo& nt) {
    io_save(file, compIndices.add(nt.nTemplate.label));  // The arbitrary id of the template
    io_save(file, nt.nTemplate.label);                   // The node name
    for (const auto& [label, component] : nt.nTemplate.components) {
      io_save(file, label == nullptr ? "" : label);
    }
    const auto [r, g, b, a] = nt.nTemplate.color;
    io_save(file, ColorToInt({r, g, b, a}));
    io_save_newline(file);
  };

  for (const auto& nt : ec.templates.registeredNodes | std::views::values) {
    if (!uniqueNodes.contains(nt.nTemplate.label)) continue;
    saveTemplate(file, nt);
  }
  for (const auto& nt : ec.templates.userDefinedNodes | std::views::values) {
    if (!uniqueNodes.contains(nt.nTemplate.label)) continue;
    saveTemplate(file, nt);
  }
}
int SaveNodes(FILE* file, EditorContext& ec) {
  io_save_section(file, "Nodes");
  int count = 0;
  for (const auto n : ec.core.nodes) {
    io_save(file, compIndices.get(n->name));
    Node::SaveState(file, *n);
    io_save_newline(file);
    count++;
  }
  return count;
}
int SaveConnections(FILE* file, EditorContext& ec) {
  io_save_section(file, "Connections");
  int count = 0;
  const auto& connections = ec.core.connections;
  for (const auto conn : connections) {
    Node& fromNode = conn->fromNode;  //Output Node
    const Component* from = conn->from;
    io_save(file, fromNode.uID);
    io_save(file, fromNode.getComponentIndex(from));
    io_save(file, fromNode.getPinIndex(from, conn->out));

    Node& toNode = conn->toNode;  //Input Node
    const Component* to = conn->to;
    io_save(file, toNode.uID);
    io_save(file, toNode.getComponentIndex(to));
    io_save(file, toNode.getPinIndex(to, conn->in));
    //End with newline
    io_save_newline(file);
    count++;
  }
  return count;
}
void LoadEditorData(FILE* file, EditorContext& ec) {
  io_load_newline(file, true);   //Skip the Editor section
  io_load_skip_separator(file);  // Node count
  io_load_skip_separator(file);  // Connection count
  io_load(file, ec.display.camera.target.x);
  io_load(file, ec.display.camera.target.y);
  io_load(file, ec.display.camera.zoom);
  io_load_newline(file);
}
void LoadTemplates(FILE* file, EditorContext& ec) {
  io_load_newline(file, false);
  int amount = 0;
  io_load(file, amount);
  io_load_newline(file, true);
  char buff[PLG_MAX_NAME_LEN];
  for (int i = 0; i < amount; ++i) {
    int index;
    io_load(file, index);
    io_load(file, buff, PLG_MAX_NAME_LEN);
    compIndices.add(buff, index);
    io_load_newline(file, true);
  }
}
int LoadNodes(FILE* file, EditorContext& ec) {
  int count = 0;
  while (io_load_inside_section(file, "Nodes")) {
    int index = -1;
    io_load(file, index);
    if (index == -1) {
      io_load_newline(file, true);
      continue;
    }
    int id;
    io_load(file, id);
    auto* nodeName = compIndices.getName(index);
    const auto newNode = ec.core.createNode(ec, nodeName, {0, 0}, static_cast<uint16_t>(id));
    if (!newNode) {
      io_load_newline(file, true);
      continue;
    }
    Node::LoadState(file, *newNode);
    ec.core.UID = std::max(ec.core.UID, static_cast<NodeID>(newNode->uID + 1));
    io_load_newline(file);
    count++;
  }
  return count;
}
int LoadConnections(FILE* file, EditorContext& ec) {
  int count = 0;
  const int maxNodeID = ec.core.UID;
  while (io_load_inside_section(file, "Connections")) {
    int fromNode, from, out;
    int toNode, to, in;
    //Output
    io_load(file, fromNode);
    io_load(file, from);
    io_load(file, out);
    //Input
    io_load(file, toNode);
    io_load(file, to);
    io_load(file, in);
    if (IsValidConnection(maxNodeID, fromNode, from, out, toNode, to, in)) {
      CreateNewConnection(ec, fromNode, from, out, toNode, to, in);
    }
    io_load_newline(file);
    count++;
  }
  return count;
}
}  // namespace

bool Persist::saveProject(EditorContext& ec, bool saveAsMode) {
  // Strictly enforce this to limit saving -> Actions need to be accurate
  if (!ec.core.hasUnsavedChanges && !saveAsMode) return true;

  // If "SaveAs" we want to save with a new name - regardless of an existing one
  if (saveAsMode || openedFilePath.empty()) {
    auto* res = tinyfd_saveFileDialog("Save File", nullptr, 1, Info::fileFilter, Info::fileDescription);
    if (res != nullptr) {
      openedFilePath = res;
    } else {
      return true;
    }
  }

  const int size = std::max(static_cast<int>(ec.core.nodes.size()), 1);

  compIndices.reset();

  int nodes, connections;
  //We assume 1000 bytes on average per node for the buffer
  const auto res = io_save_buffered_write(openedFilePath.c_str(), size * 1000, [&](FILE* file) {
    SaveEditorData(file, ec);
    SaveTemplates(file, ec);
    nodes = SaveNodes(file, ec);
    connections = SaveConnections(file, ec);
  });

  if (!res) {
    fprintf(stderr, "Error saving to %s", openedFilePath.c_str());
    return false;
  }

  // Successfully saved - reflect in the title
  ec.core.hasUnsavedChanges = false;
  ec.string.updateWindowTitle(ec);

  //printf("Saved %s nodes\n", ec.string.getPaddedNum(nodes));
  //printf("Saved %s connections\n", ec.string.getPaddedNum(connections));
  return true;
}
bool Persist::importProject(EditorContext& ec) {
  if (openedFilePath.empty()) {
    //TODO save and reuse default path
    auto* res = tinyfd_openFileDialog("Open File", nullptr, 1, Info::fileFilter, Info::fileDescription, 0);
    if (res != nullptr) openedFilePath = res;
  }

  const auto* path = openedFilePath.c_str();

  FILE* file = fopen(path, "rb");

  if (file == nullptr) {
    fprintf(stderr, "Unable to open file %s\n", path);
    return false;
  }

  // Reset editor to initial state
  ec.core.resetEditor(ec);
  compIndices.reset();

  //Load data
  int nodes = 0;
  int connections = 0;
  LoadEditorData(file, ec);
  LoadTemplates(file, ec);
  nodes = LoadNodes(file, ec);
  connections = LoadConnections(file, ec);

  //printf("Loaded %s nodes\n", ec.string.getPaddedNum(nodes));
  //printf("Loaded %s connections\n", ec.string.getPaddedNum(connections));

  if (fclose(file) != 0) return false;

  // Successfully loaded - reflect in the title
  ec.string.updateWindowTitle(ec);
  fileName = GetFileName(openedFilePath.c_str());

  return true;
}

//-----------USER_FILES-----------//

namespace {
bool LoadFromFile(EditorContext& ec, const char* path, PersistFunc func) {
  FILE* file = fopen(path, "rb");

  if (file == nullptr) {
    fprintf(stderr, "Unable to open file %s\n", path);
    return true;
  }

  auto res = func(ec, file);
  if (!res) return false;

  if (fclose(file) != 0) return false;

  return true;
}

bool LoadUserTemplates(EditorContext& ec, FILE* file) {
  io_load_newline(file, true);
  int size;
  io_load(file, size);
  io_load_newline(file, true);
  char buffer[PLG_MAX_NAME_LEN]{};
  for (int i = 0; i < size; ++i) {
    NodeTemplate temp;
    auto written = io_load(file, buffer, PLG_MAX_NAME_LEN);
    if (written > 0) temp.label = str_dup(buffer);
    else {
      io_load_newline(file, true);
      continue;
    }

    for (int j = 0; j < COMPS_PER_NODE; ++j) {
      written = io_load(file, buffer, PLG_MAX_NAME_LEN);
      if (written > 0) temp.components[i].label = str_dup(buffer);
      written = io_load(file, buffer, PLG_MAX_NAME_LEN);
      if (written > 0) temp.components[i].component = str_dup(buffer);
    }

    // Node create func
    const auto createFunc = [](const NodeTemplate& nt, Vec2 p, NodeID id) {
      return new Node(nt, p, id);
    };

    ec.templates.userDefinedNodes.insert({temp.label, {temp, createFunc}});
    ec.ui.canvasContextMenu.addNode(UI::USER_CATEGORY, temp.label);
    io_load_newline(file, true);
  }
  return true;
}

bool SaveUserTemplatesImpl(EditorContext& ec, FILE* file) {
  io_save_section(file, "Templates");
  io_save(file, static_cast<int>(ec.templates.userDefinedNodes.size()));
  io_save_newline(file);
  for (const auto& nt : ec.templates.userDefinedNodes | std::views::values) {
    io_save(file, nt.nTemplate.label);  // The node name
    for (const auto& [label, component] : nt.nTemplate.components) {
      io_save(file, label == nullptr ? "" : label);
      io_save(file, component == nullptr ? "" : component);
    }
    const auto [r, g, b, a] = nt.nTemplate.color;
    io_save(file, ColorToInt({r, g, b, a}));
    io_save_newline(file);
  }
  return true;
}

}  // namespace

bool Persist::loadUserFiles(EditorContext& ec) {
  Constraint<true> c;

  c + ChangeDirectory(GetApplicationDirectory());
  c + LoadFromFile(ec, Info::userTemplates, [](EditorContext& ec, FILE* file) {
    return LoadUserTemplates(ec, file);
  });

  return c.holds();
}

bool Persist::saveUserTemplates(EditorContext& ec) {
  constexpr int sizePerTemplate = 250;
  const int size = std::max(static_cast<int>(ec.templates.userDefinedNodes.size()), 1) * sizePerTemplate;

  const auto res =
      io_save_buffered_write(Info::userTemplates, size, [&](FILE* file) { SaveUserTemplatesImpl(ec, file); });

  if (!res) {
    fprintf(stderr, "Error saving to %s", Info::userTemplates);
    return false;
  }

  return true;
}