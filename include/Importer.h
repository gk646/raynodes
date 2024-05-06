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

#ifndef IMPORTER_H
#define IMPORTER_H

#include <vector>
#include <string>
#include <array>
#include <cstdio>

// This is a Cross-Platform, Single Header, No Dependencies, util header for importing the ".rn" format
// Its meant to be a optimized and memory efficient abstraction over raw parsing
// This interface provide easy and structured random access to connections, nodes, components and their data
// The focus is on reducing allocations and memory usage with more runtime overhead

// Current memory footprint (bytes):
// Total Memory = FileSize + nodeCount * NodeData(32) + connectionCount * ConnectionData(8) + Misc(100)(vectors,variables...)
// ComponentData = COMPONENTS_PER_NODE * ComponentData(4) + NameHash(4)

namespace raynodes {
class RnImport;

// Returns a built import of a ".rn" file as generated from raynodes
// "path" can be relative (to the current CWD) or absolute
RnImport ParseRN(const char* path);

#define COMPS_PER_NODE 6
#define DATAPOINTS_PER_COMPONENT 4
#define USED_LINE_SEPARATOR '|'  // Set this to the linesep used by cxio (default '|')

using ByteIndex = uint32_t;
using NodeID = uint16_t;
using ComponentID = uint8_t;

enum class DataType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
  VECTOR_3,
  VECTOR_2,
};

struct ConnectionData {
  NodeID fromNode;
  uint8_t fromComponent;
  uint8_t fromPin;
  NodeID toNode;
  uint8_t toComponent;
  uint8_t toPin;
};

class ComponentData {
  // This uses a single 32bit number to store both the startByte and the ComponentLabel
  // Current split 24/8
  uint32_t dataHolder;
  [[nodiscard]] ComponentID getID() const {
    // Extract the lower 8 bits
    return static_cast<ComponentID>(dataHolder & 0xFF);
  }
  [[nodiscard]] ByteIndex getStartByte() const {
    // Shift right by 8 bits and then mask to get the original 24 bits
    return static_cast<ByteIndex>((dataHolder >> 8) & 0xFFFFFF);
  }

 public:
  ComponentData(ByteIndex startByte, ComponentID id) {
    // Mask and shift `startByte` into the upper 24 bits, and `id` into the lower 8 bits
    dataHolder = (static_cast<uint32_t>(startByte) & 0xFFFFFF) << 8;
    dataHolder |= static_cast<uint32_t>(id) & 0xFF;
  }

  // Returns the data of the index-th save call made inside the component - 0 based
  // io_save(file, first), io_save(file, second), io_save(file, third);
  // IMPORTANT: strings will be returned as allocated "char*"
  template <DataType dt>
  auto getData(char* fileData, int index) const;
};

struct NodeData {
  NodeID nodeID = 0;
  uint32_t typeHash = 0;
  std::array<ComponentData, COMPS_PER_NODE> compData;
};

class RnImport final {
  std::vector<NodeData> nodes;              // Internal data holder
  std::vector<ConnectionData> connections;  // Internal data holder

  [[nodiscard]] const NodeData& getNodeData(NodeID nodeID) const {
    for (const auto& n : nodes) {
      if (n.nodeID == nodeID) return n;
    }
    std::abort();  // Node not found!
  }
  void buildInternals();

 public:
  RnImport(char* fileData, ByteIndex size) : size(size), fileData(fileData) {
    const int nodeSizeEstimate = static_cast<int>(static_cast<float>(size) * 0.75F);
    nodes.reserve(nodeSizeEstimate / 100);
    connections.reserve((size - nodeSizeEstimate) / 50);
    buildInternals();
  }
  RnImport(const RnImport& other) = delete;
  RnImport(RnImport&& other) noexcept = delete;
  RnImport& operator=(const RnImport& other) = delete;
  RnImport& operator=(RnImport&& other) noexcept = delete;
  ~RnImport() { free(fileData); }

  // Returns the data of the index-th save call made inside the specified component - 0 based
  // io_save(file, first), io_save(file, second), io_save(file, third);
  // Most components will probably only have have 1 - its up to the user to know the correct datatype
  // IMPORTANT: strings will be returned as allocated "char*"
  template <DataType dt>
  [[nodiscard]] auto getComponentData(NodeID nodeID, int component, int index = 0) const;

  // Returns the ID of the first found node thats connected to this component
  // Useful if a component only has 1 connection
  // Returns UINT16_MAX if no node is found
  [[nodiscard]] NodeID getFirstConnectedNode(NodeID nodeID, int componentIndex) const;

  // Returns the IDs of nodes that the component has a connection to
  // Stops if the specified size is reached or pads it with UINT16_MAX if not reached
  template <int size>
  [[nodiscard]] std::array<NodeID, size> getConnectedNodes(NodeID nodeID, int componentIndex) const;

  //TODO build save indicies to read - components names and datatypes

  uint32_t size = 0;         // Size of fileData
  char* fileData = nullptr;  // Allocated string containing the whole file data
};
}  // namespace raynodes

//--------------IMPLEMENTATION---------------//

namespace {
void SkipSeparator(char*& ptr, char separator, int count) noexcept {
  while (*ptr != '\0' && count > 0) {
    if (*ptr == separator) {
      --count;
    }
    ++ptr;
  }
}
constexpr uint32_t fnv1a_32(char const* s, const size_t count) noexcept {
  uint32_t hash = 2166136261u;
  for (size_t i = 0; i < count; ++i) {
    hash ^= (uint32_t)s[i];
    hash *= 16777619u;
  }
  return hash;
}
}  // namespace

inline raynodes::RnImport raynodes::ParseRN(const char* path) {
  FILE* file;
  fopen_s(&file, path, "rb");  // Open in binary mode to avoid text translation
  if (!file) {
    perror("Failed to open file securely");
  }

  // Seek to the end to find the size of the file
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek in file");
    fclose(file);
  }

  long fileSize = ftell(file);
  if (fileSize == -1) {
    perror("Failed to determine file size");
    fclose(file);
  }

  auto size = static_cast<size_t>(fileSize);

  // Return to the beginning of the file
  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to file start");
    fclose(file);
  }

  // Allocate memory for the file content
  auto* buffer = static_cast<char*>(malloc(size + 1));  // +1 for null terminator
  if (buffer == nullptr) {
    perror("Failed to allocate memory");
    fclose(file);
  }

  // Read the file into the buffer
  if (fread(buffer, 1, size, file) != size) {
    perror("Failed to read the file completely");
    free(buffer);
    fclose(file);
  }

  if (buffer) buffer[size] = '\0';  // Null-terminate the buffer

  // Close the file
  fclose(file);

  // Return the result
  return {buffer, static_cast<ByteIndex>(size)};
}

inline void raynodes::RnImport::buildInternals() {}

inline raynodes::NodeID raynodes::RnImport::getFirstConnectedNode(NodeID nodeID, int componentIndex) const {
  for (const auto& conn : connections) {
    if (conn.fromNode == nodeID && conn.fromComponent == componentIndex) return conn.toNode;
  }
  return UINT16_MAX;
}

template <raynodes::DataType dt>
auto raynodes::ComponentData::getData(char* fileData, int index) const {
  uint32_t startByte = getStartByte();
  SkipSeparator(fileData, USED_LINE_SEPARATOR, index);
  if constexpr (dt == DataType::BOOLEAN) {
    return std::strtol(fileData + startByte, nullptr, 10) == 1;
  } else if (dt == DataType::STRING) {
    char* start = fileData + startByte;
    int count = 0;
    while (*(start + count) != USED_LINE_SEPARATOR && *(start + count) != '\0') {
      count++;
    }
    const char temp = *(start + count);
    *(start + count) = '\0';
    char* allocatedString = _strdup(start);
    *(start + count) = temp;
    return allocatedString;
  } else if (dt == DataType::INTEGER) {
    return std::strtoll(fileData + startByte, nullptr, 10);
  } else if (dt == DataType::FLOAT) {
    return std::strtof(fileData + startByte, nullptr);
  }
}

template <raynodes::DataType dt>
auto raynodes::RnImport::getComponentData(NodeID nodeID, int component, int index) const {
  return getNodeData(nodeID).compData[component].getData<dt>(fileData, index);
}

template <int size>
std::array<raynodes::NodeID, size> raynodes::RnImport::getConnectedNodes(NodeID nodeID,
                                                                         int componentIndex) const {
  int i = 0;
  std::array<NodeID, size> ids;
  for (const auto& conn : connections) {
    if (conn.fromNode == nodeID && conn.fromComponent == componentIndex) {
      ids[i++] = conn.toNode;
      if (i == size) return ids;
    }
  }
  while (i < size) {
    ids[i++] = UINT16_MAX;
  }
  return ids;
}

#endif  //IMPORTER_H