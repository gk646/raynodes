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

#include <cxutil/cxio.h>
#include <cxstructs/Constraint.h>
#include <tinyfiledialogs.h>

//TODO make save format that combines both saving and loading
//-> should be easy as its symmetric - function wrapper and boolean for loading-saving

namespace {
void SaveEditorData(FILE* file, EditorContext& ec) {
  //TODO save more data
  cxstructs::io_save_section(file, "EditorData");
  cxstructs::io_save(file, ec.display.camera.target.x);
  cxstructs::io_save(file, ec.display.camera.target.y);
  cxstructs::io_save(file, ec.display.camera.zoom);
  cxstructs::io_save(file, ec.ui.showGrid);
  cxstructs::io_save(file, static_cast<int>(ec.core.nodes.size()));        // Total nodes
  cxstructs::io_save(file, static_cast<int>(ec.core.connections.size()));  // Total connections
  cxstructs::io_save_newline(file);
}
int SaveNodes(FILE* file, EditorContext& ec) {
  cxstructs::io_save_section(file, "Nodes");
  int count = 0;
  for (const auto n : ec.core.nodes) {
    Node::SaveState(file, *n);
    cxstructs::io_save_newline(file);
    count++;
  }
  return count;
}
int SaveConnections(FILE* file, EditorContext& ec) {
  cxstructs::io_save_section(file, "Connections");
  int count = 0;
  const auto& connections = ec.core.connections;
  for (const auto conn : connections) {
    Node& fromNode = conn->fromNode;  //Output Node
    const Component* from = conn->from;
    cxstructs::io_save(file, fromNode.uID);
    cxstructs::io_save(file, fromNode.getComponentIndex(from));
    cxstructs::io_save(file, fromNode.getPinIndex(from, conn->out));

    Node& toNode = conn->toNode;  //Input Node
    const Component* to = conn->to;
    cxstructs::io_save(file, toNode.uID);
    cxstructs::io_save(file, toNode.getComponentIndex(to));
    cxstructs::io_save(file, toNode.getPinIndex(to, conn->in));
    //End with newline
    cxstructs::io_save_newline(file);
    count++;
  }
  return count;
}
void LoadEditorData(FILE* file, EditorContext& ec) {
  cxstructs::io_load_newline(file, true);  //Skip the Editor section
  cxstructs::io_load(file, ec.display.camera.target.x);
  cxstructs::io_load(file, ec.display.camera.target.y);
  cxstructs::io_load(file, ec.display.camera.zoom);
  cxstructs::io_load(file, ec.ui.showGrid);
  cxstructs::io_load_newline(file);  // Node count
  cxstructs::io_load_newline(file);  // Connection count

  cxstructs::io_load_newline(file);
}
int LoadNodes(FILE* file, EditorContext& ec) {
  int count = 0;
  char buff[Plugin::MAX_NAME_LEN];
  while (cxstructs::io_load_inside_section(file, "Nodes")) {
    int id= 0;
    cxstructs::io_load(file, buff, Plugin::MAX_NAME_LEN);
    cxstructs::io_load(file, id);
    const auto newNode = ec.core.createNode(ec, buff, {0, 0}, static_cast<uint16_t>(id));
    if (!newNode) {
      cxstructs::io_load_newline(file, true);
      continue;
    }
    Node::LoadState(file, *newNode);
    ec.core.UID = std::max(ec.core.UID, static_cast<NodeID>(newNode->uID + 1));
    cxstructs::io_load_newline(file);
    count++;
  }
  return count;
}

bool IsValidConnection(int maxNodeID, int fromNode, int from, int out, int toNode, int to, int in) {
  const bool correctNode = fromNode >= 0 && fromNode < maxNodeID && toNode >= 0 && toNode < maxNodeID;
  const bool correctFromComponent = (from >= 0 && from < COMPS_PER_NODE) || from == -1;
  const bool correctToComponent = (to >= 0 && to < COMPS_PER_NODE) || to == -1;
  return correctNode && correctFromComponent && correctToComponent && out != -1 && in != -1;
}

void CreateNewConnection(EditorContext& ec, int fromNodeID, int fromI, int outI, int toNodeID, int toI,
                         int inI) {
  const auto& nodeMap = ec.core.nodeMap;
  // We use int8_t as size_type to save space
  Node& fromNode = *nodeMap.at(static_cast<NodeID>(fromNodeID));

  Component* from = nullptr;
  OutputPin* out;
  if (fromI != -1) {
    from = fromNode.components[static_cast<int8_t>(fromI)];
    out = &from->outputs[static_cast<int8_t>(outI)];
  } else {
    out = &fromNode.outputs[static_cast<int8_t>(outI)];
  }

  Node& toNode = *nodeMap.at(static_cast<NodeID>(toNodeID));
  Component* to = nullptr;
  InputPin* in = nullptr;
  if (toI != -1) {
    to = toNode.components[static_cast<int8_t>(toI)];
    in = &to->inputs[static_cast<int8_t>(inI)];
  } else if (inI == 0) {
    in = &toNode.nodeIn;
  }

  ec.core.addConnection(new Connection(fromNode, from, *out, toNode, to, *in));
}

int LoadConnections(FILE* file, EditorContext& ec) {
  int count = 0;
  const int maxNodeID = ec.core.UID;
  while (cxstructs::io_load_inside_section(file, "Connections")) {
    int fromNode, from, out;
    int toNode, to, in;
    //Output
    cxstructs::io_load(file, fromNode);
    cxstructs::io_load(file, from);
    cxstructs::io_load(file, out);
    //Input
    cxstructs::io_load(file, toNode);
    cxstructs::io_load(file, to);
    cxstructs::io_load(file, in);
    if (IsValidConnection(maxNodeID, fromNode, from, out, toNode, to, in)) {
      CreateNewConnection(ec, fromNode, from, out, toNode, to, in);
    }
    cxstructs::io_load_newline(file);
    count++;
  }
  return count;
}
}  // namespace

bool Persist::saveToFile(EditorContext& ec, bool saveAsMode) {
  // Strictly enforce this to limit saving -> Actions need to be accurate
  if (!ec.core.hasUnsavedChanges && !saveAsMode) return true;

  // If "SaveAs" we want to save with a new name - regardless of an existing one
  if (openedFilePath.empty() || saveAsMode) {
    auto* res = tinyfd_saveFileDialog("Save File", nullptr, 1, Info::fileFilter, Info::fileDescription);
    if (res != nullptr) {
      openedFilePath = res;
    } else {
      return true;
    }
  }

  const int size = std::max(static_cast<int>(ec.core.nodes.size()), 1);

  int nodes, connections;
  //We assume 1000 bytes on average per node for the buffer
  const auto res = cxstructs::io_save_buffered_write(openedFilePath.c_str(), size * 1000, [&](FILE* file) {
    SaveEditorData(file, ec);
    nodes = SaveNodes(file, ec);
    connections = SaveConnections(file, ec);
  });

  if (!res) {
    fprintf(stderr, "Error saving to %s", openedFilePath.c_str());
    return false;
  }

  // Successfully saved - reflect in the title
  ec.core.hasUnsavedChanges = false;
  String::updateWindowTitle(ec);

  printf("Saved %s nodes\n", String::GetPaddedNum(nodes));
  printf("Saved %s connections\n", String::GetPaddedNum(connections));
  return true;
}

bool Persist::loadFromFile(EditorContext& ec) {
  FILE* file;

  if (openedFilePath.empty()) {
    //TODO save and reuse default path
    auto* res = tinyfd_openFileDialog("Open File", nullptr, 1, Info::fileFilter, Info::fileDescription, 0);
    if (res != nullptr) openedFilePath = res;
  }

  const auto* path = openedFilePath.c_str();

  fopen_s(&file,path, "rb");

  if (file == nullptr) {
    fprintf(stderr, "Unable to open file %s\n", path);
    return false;
  }

  // Reset editor to initial state
  ec.core.resetEditor(ec);

  //Load data
  LoadEditorData(file, ec);
  int nodes = LoadNodes(file, ec);
  int connections = LoadConnections(file, ec);

  printf("Loaded %s nodes\n", String::GetPaddedNum(nodes));
  printf("Loaded %s connections\n", String::GetPaddedNum(connections));

  if (fclose(file) != 0) return false;

  // Successfully loaded - reflect in the title
  String::updateWindowTitle(ec);
  fileName = GetFileName(openedFilePath.c_str());

  return true;
}

bool Persist::loadWorkingDirectory(EditorContext& /**/) {
  cxstructs::Constraint<true> c;

  c + ChangeDirectory(GetApplicationDirectory());

  return c.holds();
}