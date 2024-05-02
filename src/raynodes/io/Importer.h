#ifndef IMPORTER_H
#define IMPORTER_H

#include <vector>
#include <string>
#include <array>
#include <corecrt_io.h>
#include <fcntl.h>

// This is meant to be included in any project using a ".rn" save file
// The focus here is on reducing allocations and memory usage with more runtime overhead

namespace raynodes {
#define MAX_NODES_PER_COMPONENT 6
#define USED_LINE_SEPARATOR '|'  // Set this to the linesep used by cxio (default '|')
using IndexType = uint32_t;      // Adjust this if your dealing with small files (below 65kb use uin16_t)
using NodeID = uint16_t;

enum class DataType : uint8_t {
  BOOLEAN,
  STRING,
  INTEGER,
  DATA,
  FLOAT,
  VECTOR_3,
  VECTOR_2,
};

struct ComponentData {
  IndexType startIndex = 0;
  uint32_t nameHash = 0;  // Hash of the component label  //TODO add error message on save if collisions

  // Returns the requested data by value
  // IMPORTANT: strings will be returned as allocated "char*"
  template <DataType dt>
  auto getData(char* fileBuffer) const {
    if constexpr (dt == DataType::BOOLEAN) {
      return std::strtol(fileBuffer + startIndex, nullptr, 10) == 1;
    } else if (dt == DataType::STRING) {
      char* start = fileBuffer + startIndex;
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
      return std::strtoll(fileBuffer + startIndex, nullptr, 10);
    } else if (dt == DataType::FLOAT) {
      return std::strtof(fileBuffer + startIndex, nullptr);
    }
  }
  [[nodiscard]] bool isValid() const { return startIndex != 0; }
};

struct NodeData {
  NodeID nodeID = 0;
  std::array<ComponentData, MAX_NODES_PER_COMPONENT> compData;
};

struct ConnectionData {
  NodeID fromNode;
  uint8_t fromComponent;
  uint8_t fromPin;
  NodeID toNode;
  uint8_t toComponent;
  uint8_t toPin;
};

class RnImport {
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
  RnImport(char* fileData, IndexType size) : size(size), fileData(fileData) {
    int nodeSizeEstimate = size * 0.75;
    nodes.reserve(nodeSizeEstimate / 100);
    connections.reserve((size - nodeSizeEstimate) / 50);
    buildInternals();
  }
  RnImport(const RnImport& other) = delete;
  RnImport(RnImport&& other) noexcept = delete;
  RnImport& operator=(const RnImport& other) = delete;
  RnImport& operator=(RnImport&& other) noexcept = delete;
  ~RnImport() { free(fileData); }

  // Returns the data of the specified component based on the datatype
  // Its up to the user to know the correct datatype
  // IMPORTANT: String returns an alloated terminated char*
  template <DataType dt>
  [[nodiscard]] auto getComponentData(NodeID nodeID, int componentIndex) const {
    return getNodeData(nodeID).compData[componentIndex].getData<dt>(fileData);
  }

  // Returns the ID of the first found node thats connected to this component
  // Useful if a component only has 1 connection
  // Returns UINT16_MAX if no node is found
  [[nodiscard]] NodeID getFirstConnectedNode(NodeID nodeID, int componentIndex) const {
    for (const auto& conn : connections) {
      if (conn.fromNode == nodeID && conn.fromComponent == componentIndex) return conn.toNode;
    }
    return UINT16_MAX;
  }

  // Returns the IDs of nodes that the component has a connection to
  // Stops if the specified size is reached or pads it with UINT16_MAX if not reached
  template <int size>
  [[nodiscard]] std::array<NodeID, size> getConnectedNodes(NodeID nodeID, int componentIndex) const {
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

  IndexType size = 0;        // Size of fileData
  char* fileData = nullptr;  // Allocated string containing the whole file data
};

// Returns a built import of a ".rn" file as generated from raynodes
inline RnImport parseRN(const char* path) {
  int fd;
  errno_t err;
  // Open the file securely
  err = _sopen_s(&fd, path, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD);
  if (err != 0 || fd == -1) {
    perror("Failed to open file securely");
    exit(EXIT_FAILURE);
  }

  // Get the size of the file
  off_t size = _lseek(fd, 0, SEEK_END);
  if (size == -1) {
    _close(fd);
    perror("Failed to determine file size");
    exit(EXIT_FAILURE);
  }

  // Return to the beginning of the file
  if (_lseek(fd, 0, SEEK_SET) == -1) {
    _close(fd);
    perror("Failed to seek to file start");
    exit(EXIT_FAILURE);
  }

  // Allocate memory for the file content
  auto* buffer = static_cast<char*>(malloc(size + 1));  // +1 for null terminator
  if (buffer == nullptr) {
    _close(fd);
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  // Read the file into the buffer
  auto read_size = _read(fd, buffer, size);

  if (read_size != size) {
    free(buffer);
    _close(fd);
    perror("Failed to read the file completely");
    exit(EXIT_FAILURE);
  }
  buffer[size] = '\0';  // Null-terminate the string

  // Close the file
  _close(fd);

  // Return the result
  return {buffer, static_cast<IndexType>(size)};
}
}  // namespace raynodes

// Internal Implementation
inline void raynodes::RnImport::buildInternals() {}

#endif  //IMPORTER_H