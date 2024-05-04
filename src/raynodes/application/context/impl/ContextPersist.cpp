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

namespace {
void SaveEditorData(FILE* file, EditorContext& ec) {
  //TODO make save format that generate both saving and loading
  //TODO save more data
  cxstructs::io_save_section(file, "EditorData");
  cxstructs::io_save(file, ec.display.camera.target.x);
  cxstructs::io_save(file, ec.display.camera.target.y);
  cxstructs::io_save(file, ec.display.camera.zoom);
  cxstructs::io_save_newline(file);
}
int SaveNodes(FILE* file, EditorContext& ec) {
  cxstructs::io_save_section(file, "Nodes");
  int count = 0;
  for (const auto n : ec.core.nodes) {
    n->saveState(file);
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
    Component& from = conn->from;
    cxstructs::io_save(file, fromNode.id);
    cxstructs::io_save(file, fromNode.getComponentIndex(from));
    cxstructs::io_save(file, from.getPinIndex(&conn->out));

    Node& toNode = conn->toNode;  //Input Node
    Component& to = conn->to;
    cxstructs::io_save(file, toNode.id);
    cxstructs::io_save(file, toNode.getComponentIndex(to));
    cxstructs::io_save(file, to.getPinIndex(&conn->in));
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
  cxstructs::io_load_newline(file);
}
int LoadNodes(FILE* file, EditorContext& ec) {
  int count = 0;
  char buff[Plugin::MAX_NAME_LEN];
  while (cxstructs::io_load_inside_section(file, "Nodes")) {
    int id;
    cxstructs::io_load(file, buff, Plugin::MAX_NAME_LEN);
    cxstructs::io_load(file, id);
    const auto newNode = ec.core.createNode(ec, buff, {0, 0}, id);
    if (!newNode) {
      cxstructs::io_load_newline(file, true);
      continue;
    }
    newNode->loadState(file);
    ec.core.UID = std::max(ec.core.UID, static_cast<NodeID>(newNode->id + 1));
    cxstructs::io_load_newline(file);
    count++;
  }
  return count;
}
bool IsValidConnection(int maxNodeID, int fromNode, int from, int out, int toNode, int to, int in) {
  return fromNode > 0 && fromNode < maxNodeID && from != -1 && out != -1 && toNode > 0 && toNode < maxNodeID
         && to != -1 && in != -1;
}
void CreateNewConnection(EditorContext& ec, int fromNodeID, int fromI, int outI, int toNodeID, int toI,
                         int inI) {
  const auto& nodeMap = ec.core.nodeMap;
  Node& fromNode = *nodeMap.at(NodeID(fromNodeID));
  Component& from = *fromNode.components[fromI];
  OutputPin& out = from.outputs[outI];

  Node& toNode = *nodeMap.at(NodeID(toNodeID));
  Component& to = *toNode.components[toI];
  InputPin& in = to.inputs[inI];

  ec.core.addConnection(new Connection(fromNode, from, out, toNode, to, in));
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

bool Persist::saveToFile(EditorContext& ec) {
  // Strictly enforce this to limit saving -> Actions need to be accurate
  if (!ec.core.hasUnsavedChanges) return true;

  if (openedFilePath.empty()) {
    auto* res = tinyfd_saveFileDialog("Save File", nullptr, 1, Info::fileFilter, Info::fileDescription);
    if (res != nullptr) {
      openedFilePath = res;
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

  if (fopen_s(&file, path, "r") != 0) return false;

  if (!file) {
    fprintf(stderr, "Unable to open file %s\n", path);
    return false;
  }

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

bool Persist::loadWorkingDirectory(EditorContext& ec) {
  cxstructs::Constraint<true> c;

  c + ChangeDirectory(GetApplicationDirectory());

  return c.holds();
}