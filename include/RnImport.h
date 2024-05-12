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

#include <string>
#include <vector>
#include <array>
#include <cstring>  // for strlen() on gcc

// ==============================
// IMPORT INTERFACE
// ==============================

// This is a Cross-Platform, Single Header, No Dependencies, util header for importing the ".rn" format
// Its meant to be a optimized and memory efficient abstraction over raw parsing
// This interface provide easy and structured random access to connections, nodes, components and their data
// The focus is on reducing memory footprint with more runtime overhead
// For most operations it provides both allocating and non allocating return containers (vector or array)
// ................................................................................................
// It basically just does one pass and saves byte markers where information is stored within the file
// This later allows for fast, random and frequent access without any more internal allocations
// ................................................................................................
// The RnImport interface also abstracts away the fileType so new filetypes like JSON or .csv can be added

// Current memory footprint thats dynamically allocated (bytes):
// Total Memory = FileSize + nodes * NodeData(8) + connections * ConnectionData(8) + nodeTypes * NodeTemplate(2)

namespace raynodes {
struct RnImport;

// Returns a built import of a ".rn" file as generated from raynodes
// "path" can be relative (to the current CWD) or absolute
RnImport importRN(const char* path);

#define COMPS_PER_NODE 6         // Max components per node
#define USED_LINE_SEPARATOR '|'  // Set this to the linesep used by cxio (default '|')
#define COMPONENT_SEPARATOR '$'  // The separator between component data (default '$')
#define RN_MAX_NAME_LEN 16       // Max length of any component or node identifiers

using ByteIndex = uint32_t;      // Limits fileSize to 4GB
using NodeID = uint16_t;         // Limits to 65536 nodes per project
using ComponentIndex = uint8_t;  // Limits to 256 components per node
using TemplateID = uint8_t;      // Limits to 256 unique nodes

// Dont wanna include raylib everywhere
struct Vec2 final {
  float x;
  float y;
};

// This way we can return a unallocated string
struct StringView final{
  const char* start = nullptr;
  uint16_t length = 0;
  [[nodiscard]] std::string getString() const {
     if(start == nullptr || length == 0) [[unlikely]]{
     return std::string() ;
     }
    return {start, length};
  };

struct Vec3 final{
  float x;
  float y;
  float z;
};

enum DataType : uint8_t {
  BOOLEAN,      // Returns a bool value
  STRING,       // Returns a std::string
  INTEGER,      // Returns a int64_t
  DATA,         // Not supported
  FLOAT,        // Returns a double
  VECTOR_3,     // Returns a Vec3
  VECTOR_2,     // Returns a Vec2
  STRING_VIEW,  // Returns a StringView - unallocated string
};

struct Connection final{
  NodeID fromNode;       // Value between 0 and nodeCnt-1 or ]0 - nodeCnt] or (0 - nodeCnt]
  int8_t fromComponent;  // -1 if its a node-to-node connection
  uint8_t fromPin;
  NodeID toNode;       // Value between 0 and nodeCnt-1 or ]0 - nodeCnt] or (0 - nodeCnt]
  int8_t toComponent;  // -1 if its a node-to-node connection
  uint8_t toPin;
  [[nodiscard]] bool isValid() const { return fromNode != UINT16_MAX; }
};

struct NodeData final {
  ByteIndex startByte = 0;
  const NodeID id = 0;
  const TemplateID tID = 0;

  template <DataType dt>
  auto getData(char* fileData, ComponentIndex id, int index) const;
};

struct NodeTemplate final{
  const uint16_t startByte = 0;
  ComponentIndex getCompIndex(char* fileData, const char* label) const;
  StringView getName(const char* fileData) const;
  bool isNodeName(const char* fileData, const char* nodeName) const;
};

// All members are openly accessible to allow custom tinkering - only use them if you know what your doing!
struct RnImport final {
  uint16_t templateCnt = 0;           // Amount of templates
  NodeTemplate* templates = nullptr;  // Node templates
  uint16_t nodeCnt = 0;               // Amount of nodes
  NodeData* nodes = nullptr;          // Internal data holder
  uint16_t connCnt = 0;               // Amount of connections
  Connection* connections = nullptr;  // Internal data holder

  char* fileData = nullptr;  // Allocated string containing the whole file data
  uint32_t size = 0;         // Size of fileData

  RnImport(char* fileData, ByteIndex size);
  RnImport(const RnImport& other) = delete;
  RnImport(RnImport&& other) noexcept = delete;
  RnImport& operator=(const RnImport& other) = delete;
  RnImport& operator=(RnImport&& other) noexcept = delete;
  ~RnImport() {
    free(fileData);
    free(templates);
    free(nodes);
    free(connections);
  }

  // Returns the data of the component with the given label from the node identified by nodeID
  // Its up to the user to know the correct type
  // This function is a bit slower than its counterpart as it has to do the name lookup
  // Optional: saveIndex specifies the position of the save call inside the component - most of the time 0
  // Failure: returns a type appropriate dummy (false, 0, 0.0F, empty string).
  // IMPORTANT: strings will be returned as std::string()
  template <DataType dt>
  [[nodiscard]] auto getComponentData(NodeID node, const char* label, int saveIndex = 0) const;

  // Returns the data of the component at the specified index within the node - 0 based indexing
  // Its up to the user to know the correct type
  // Optional: saveIndex specifies the position of the save call inside the component - most of the time 0
  // Failure: returns a type appropriate dummy (false, 0, 0.0F, empty string).
  // IMPORTANT: strings will be returned as std::string()
  template <DataType dt>
  [[nodiscard]] auto getComponentData(NodeID node, int component, int saveIndex = 0) const;

  // Returns an array of outgoing connections for a specified node
  // If `component` or `pin` is not specified, all possible connections are included
  // Use component = -1 to get the node-to-node connection pins
  // Failure: array will always be filled - empty connections will contain max values - use Connection::isValid()
  template <int size>
  [[nodiscard]] std::array<Connection, size> getConnectionsOut(NodeID node, int component = -2, int pin = -1) const;

  // Returns a vector of outgoing connections for a specified node - reserves 5 upfront
  // If `component` or `pin` is not specified, all possible connections are included
  // Use component = -1 to get the node-to-node connection pins
  // Failure: will never fail - vector is empty or filled with valid connections
  [[nodiscard]] std::vector<Connection> getConnectionsOut(NodeID node, int component = -2, int pin = -1) const;

  // Returns an array of incoming connections for a specified node
  // If `component` or `pin` is not specified, all possible connections are included
  // Use component = -1 to get the node-to-node connection pins
  // Failure: array will always be filled - empty connections will contain max values - use Connection::isValid()
  template <int size>
  [[nodiscard]] std::array<Connection, size> getConnectionsIn(NodeID node, int component = -2, int pin = -1) const;

  // Returns a vector of incoming connections for a specified node - reserves 5 upfront
  // If `component` or `pin` is not specified, all possible connections are included
  // Use component = -1 to get the node-to-node connection pins
  // Failure: will never fail - vector is empty or filled with valid connections
  [[nodiscard]] std::vector<Connection> getConnectionsIn(NodeID node, int component = -2, int pin = -1) const;

  // Returns an array of nodes matching the name
  // Failure: array will always be filled - empty values will be UINT16_MAX
  template <int size>
  [[nodiscard]] std::array<NodeID, size> getNodes(const char* name) const;

  // Returns a vector of nodes matching the name
  // Failure: array will always be filled - empty values will be UINT16_MAX
  [[nodiscard]] std::vector<NodeID> getNodes(const char* name) const;

  // Returns the name of a node as it was registered with
  // Failure: If the given id is invalid or doesnt exist or cant be mapped returns empty string
  [[nodiscard]] std::string getNodeName(NodeID node) const;

 private:
  [[nodiscard]] const NodeData* getNodeData(const NodeID id) const {
    if (id >= nodeCnt) return nullptr;
    for (int i = 0; i < nodeCnt; ++i) {
      if (nodes[i].id == id) return &nodes[i];
    }
    return nullptr;
  }
  [[nodiscard]] const NodeTemplate* getNodeTemplate(const TemplateID id) const {
    if (id >= templateCnt) [[unlikely]]
      return nullptr;
    return &templates[id];
  }
};
}  // namespace raynodes

// ==============================
// IMPLEMENTATION
// ==============================

//-----------HELPER_FUNCTIONS-----------//
// Alot of these helpers are copied from cxutil/cxstring.h
namespace {
int str_count_chars_until(const char* data, char stop, int maxCount) {
  int count = 0;
  while (count < maxCount && data[count] != '\0') {
    if (data[count] == stop) { break; }
    count++;
  }
  return count;
}
void str_read_into_until(const char* data, char* buffer, size_t buffer_size, char stop) {
  size_t count = 0;
  while (count < buffer_size - 1 && data[count] != stop && data[count] != '\0') {
    buffer[count] = data[count];
    count++;
  }
  buffer[count] = '\0';
}
void str_skip_char(char*& ptr, const char c, int count) noexcept {
  while (*ptr != '\0' && count > 0) {
    if (*ptr == c) { --count; }
    ++ptr;
  }
}
int str_parse_int(const char* str) {
  constexpr int radix = 10;
  if (str == nullptr || *str == '\0') return 0;

  int result = 0;
  bool negative = false;
  if (*str == '-') {
    negative = true;
    ++str;
  }

  while (*str != 0) {
    char digit = *str;
    int value;
    if (digit >= '0' && digit <= '9') value = digit - '0';
    else if (digit >= 'a' && digit <= 'z') value = radix + digit - 'a';
    else if (digit >= 'A' && digit <= 'Z') value = radix + digit - 'A';
    else break;

    if (value >= radix) break;

    result = result * radix + value;
    ++str;
  }

  return negative ? -result : result;
}
bool str_cmp_stop_at(const char* newArg, const char* control, int max, char stop) {
  int offSet = 0;
  while (control[offSet] != stop && offSet < max) {
    // Check if newArg ends before control reaches the stop character or max limit
    if (newArg[offSet] == '\0' || newArg[offSet] != control[offSet]) { return false; }
    offSet++;
  }
  // Returns true if newArg matches control up to the stop character
  return true;
}
template <raynodes::DataType dt>
[[nodiscard]] auto GetDefaultValue() {
  if constexpr (dt == raynodes::STRING) {
    return std::string();  // Shouldnt make an allocation as its a empty string
  } else if constexpr (dt == raynodes::INTEGER) {
    return static_cast<int64_t>(0);
  } else if constexpr (dt == raynodes::BOOLEAN) {
    return false;
  } else if constexpr (dt == raynodes::FLOAT) {
    return 0.0;
  } else if constexpr (dt == raynodes::DATA) {
    static_assert(dt == raynodes::STRING, "Wont be supported... How?");
  } else if constexpr (dt == raynodes::VECTOR_3) {
    return raynodes::Vec3{0.0F, 0.0F, 0.0F};
  } else if constexpr (dt == raynodes::VECTOR_2) {
    return raynodes::Vec2{0.0F, 0.0F};
  } else {
    static_assert(dt == raynodes::STRING, "Unsupported PinType");
  }
}
}  // namespace

//-----------HELPER_CLASSES-----------//
namespace raynodes {
template <DataType dt>
auto NodeData::getData(char* fileData, const ComponentIndex id, const int index) const {
  char* workPtr = fileData + startByte;
  str_skip_char(workPtr, USED_LINE_SEPARATOR, 1);  // Skip the node id

  str_skip_char(workPtr, COMPONENT_SEPARATOR, id);     // Skipped if component is 0
  str_skip_char(workPtr, USED_LINE_SEPARATOR, index);  // Skipped if index is 0

  if constexpr (dt == BOOLEAN) {
    return std::strtol(workPtr, nullptr, 10) == 1;
  } else if constexpr (dt == STRING) {
    const char* start = workPtr;
    int count = 0;
    while (*(start + count) != USED_LINE_SEPARATOR && *(start + count) != '\0') {
      count++;
    }
    std::string allocatedString(start, count);
    return allocatedString;  // Rely on RVO for efficient return
  } else if constexpr (dt == INTEGER) {
    return std::strtoll(workPtr, nullptr, 10);
  } else if constexpr (dt == FLOAT) {
    return std::strtod(workPtr, nullptr);
  } else if constexpr (dt == VECTOR_2) {
    static_assert(dt == STRING, "Not yet supported");
  } else if constexpr (dt == VECTOR_3) {
    static_assert(dt == STRING, "Not yet supported");
  } else if constexpr (dt == DATA) {
    static_assert(dt == STRING, "Wont be supported... How?");
  }
}
inline ComponentIndex NodeTemplate::getCompIndex(char* fileData, const char* label) const {
  char* workPtr = fileData + startByte;
  str_skip_char(workPtr, USED_LINE_SEPARATOR, 1);
  for (uint8_t i = 0; i < COMPS_PER_NODE; ++i) {
    if (workPtr[0] == '\n') return UINT8_MAX;  // Dummy value
    if (str_cmp_stop_at(label, workPtr, RN_MAX_NAME_LEN, USED_LINE_SEPARATOR)) return i;
    str_skip_char(workPtr, USED_LINE_SEPARATOR, 1);
  }
  return UINT8_MAX;  // Dummy value
}

inline bool NodeTemplate::isNodeName(const char* fileData, const char* nodeName) const {
  return str_cmp_stop_at(nodeName, fileData + startByte, RN_MAX_NAME_LEN, USED_LINE_SEPARATOR);
}
inline StringView NodeTemplate::getName(const char* fileData) const {
  uint16_t len = str_count_chars_until(fileData + startByte, USED_LINE_SEPARATOR, RN_MAX_NAME_LEN);
  return {fileData + startByte, len};
}
}  // namespace raynodes

//-----------RN_IMPORT-----------//
namespace raynodes {
inline RnImport importRN(const char* path) {
  FILE* file = fopen(path, "rb");  // Open in binary mode to avoid text translation
  if (file == nullptr) {
    perror("Failed to open file securely");
    return {nullptr, 0};
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
inline RnImport::RnImport(char* fileData, ByteIndex size) : fileData(fileData), size(size) {
  if (fileData == nullptr) return;
  auto* indexPtr = fileData;
  // Allocating space
  {
    // This is a fixed format header - EditorData will always be 1 line e.g.:
    //--EditorData--
    //2|1|-145.238|-170.635|0.960|1|
    //--Templates--
    //10|

    str_skip_char(indexPtr, '\n', 1);
    nodeCnt = str_parse_int(indexPtr);
    str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
    connCnt = str_parse_int(indexPtr);
    str_skip_char(indexPtr, '\n', 2);
    templateCnt = str_parse_int(indexPtr);

    nodes = static_cast<NodeData*>(malloc(nodeCnt * sizeof(NodeData)));
    connections = static_cast<Connection*>(malloc(connCnt * sizeof(Connection)));
    templates = static_cast<NodeTemplate*>(malloc(templateCnt * sizeof(NodeTemplate)));
  }
  // Parsing the templates
  {
    str_skip_char(indexPtr, '\n', 1);  // Skip the template count
    for (int i = 0; i < templateCnt; ++i) {
      const int index = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      NodeTemplate nt{static_cast<uint16_t>(indexPtr - fileData)};
      memcpy(templates + index, &nt, sizeof(NodeTemplate));
      str_skip_char(indexPtr, '\n', 1);  // Copy cause of const
    }
  }
  // Parsing the nodes
  {
    str_skip_char(indexPtr, '\n', 1);  // Skip nodes section
    for (int i = 0; i < nodeCnt; ++i) {
      const int templateNum = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int nodeID = str_parse_int(indexPtr);
      NodeData nd{static_cast<ByteIndex>(indexPtr - fileData), (NodeID)nodeID, (TemplateID)templateNum};
      memcpy(nodes + i, &nd, sizeof(NodeData));  // Copy cause of const
      str_skip_char(indexPtr, '\n', 1);
    }
  }
  // Parsing connections
  {
    str_skip_char(indexPtr, '\n', 1);  // Skip connection section

    for (int i = 0; i < connCnt; ++i) {
      const int fromNode = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int from = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int out = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int toNode = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int to = str_parse_int(indexPtr);
      str_skip_char(indexPtr, USED_LINE_SEPARATOR, 1);
      const int in = str_parse_int(indexPtr);
      connections[i] = {(NodeID)fromNode, (int8_t)from, (uint8_t)out, (NodeID)toNode, (int8_t)to, (uint8_t)in};
      str_skip_char(indexPtr, '\n', 1);
    }
  }
}
template <DataType dt>
auto RnImport::getComponentData(const NodeID node, const char* label, const int saveIndex) const {
  if (label == nullptr || saveIndex < 0) [[unlikely]] { return GetDefaultValue<dt>(); }
  const auto* nData = getNodeData(node);
  if (nData == nullptr) [[unlikely]] { return GetDefaultValue<dt>(); }
  const auto* nTemplate = getNodeTemplate(nData->tID);
  if (nTemplate == nullptr) [[unlikely]] { return GetDefaultValue<dt>(); }
  ComponentIndex compIndex = nTemplate->getCompIndex(fileData, label);
  if (compIndex == UINT8_MAX) [[unlikely]] { return GetDefaultValue<dt>(); }
  return nData->getData<dt>(fileData, compIndex, saveIndex);
}
template <DataType dt>
auto RnImport::getComponentData(const NodeID node, const int component, const int saveIndex) const {
  if (component < 0 || saveIndex < 0) [[unlikely]] { return GetDefaultValue<dt>(); }
  const auto* nData = getNodeData(node);
  if (nData == nullptr) [[unlikely]] { return GetDefaultValue<dt>(); }
  return nData->getData<dt>(fileData, component, saveIndex);
}
template <int size>
std::array<Connection, size> RnImport::getConnectionsOut(const NodeID node, const int component, int pin) const {
  int index = 0;
  std::array<Connection, size> retval;
  for (int i = 0; i < connCnt; ++i) {
    if (connections[i].fromNode != node) [[likely]] { continue; }
    if (component != -2 && connections[i].fromComponent != component) [[likely]] { continue; }
    if (pin != -1 && connections[i].fromPin != pin) [[likely]] { continue; }
    retval[index++] = connections[i];
    if (index >= size) [[unlikely]] { break; }
  }
  while (index < size) [[likely]] {
    retval[index++] = {UINT16_MAX, INT8_MAX, UINT8_MAX, UINT16_MAX, INT8_MAX, UINT8_MAX};
  }
  return retval;
}
inline std::vector<Connection> RnImport::getConnectionsOut(NodeID node, int component, int pin) const {
  std::vector<Connection> retval;
  retval.reserve(5);
  for (int i = 0; i < connCnt; ++i) {
    if (connections[i].fromNode != node) [[likely]] { continue; }
    if (component != -2 && connections[i].fromComponent != component) [[likely]] { continue; }
    if (pin != -1 && connections[i].fromPin != pin) [[likely]] { continue; }
    retval.push_back(connections[i]);
  }
  return retval;
}
template <int size>
std::array<Connection, size> RnImport::getConnectionsIn(NodeID node, int component, int pin) const {
  int index = 0;
  std::array<Connection, size> retval;

  for (int i = 0; i < connCnt; ++i) {
    if (connections[i].toNode != node) [[likely]] { continue; }
    if (component != -2 && connections[i].toComponent != component) [[likely]] { continue; }
    if (pin != -1 && connections[i].toPin != pin) [[likely]] { continue; }
    retval[index++] = connections[i];
    if (index >= size) [[unlikely]] { break; }
  }
  while (index < size) [[likely]] {
    retval[index++] = {UINT16_MAX, INT8_MAX, UINT8_MAX, UINT16_MAX, INT8_MAX, UINT8_MAX};
  }

  return retval;
}
inline std::vector<Connection> RnImport::getConnectionsIn(NodeID node, int component, int pin) const {
  std::vector<Connection> retval;
  retval.reserve(5);
  for (int i = 0; i < connCnt; ++i) {
    if (connections[i].toNode != node) [[likely]] { continue; }
    if (component != -2 && connections[i].toComponent != component) [[likely]] { continue; }
    if (pin != -1 && connections[i].toPin != pin) [[likely]] { continue; }
    retval.push_back(connections[i]);
  }
  return retval;
}
template <int size>
std::array<NodeID, size> RnImport::getNodes(const char* name) const {
  int index = 0;
  std::array<NodeID, size> retVal;
  std::memset(&retVal, 255, size * sizeof(NodeID));  // Setting all bytes to 1's -> biggest possible value
  if (name == nullptr) return retVal;
  TemplateID id = UINT8_MAX;
  for (int i = 0; i < templateCnt; ++i) {
    if (templates[i].isNodeName(fileData, name)) [[unlikely]] {
      id = static_cast<uint8_t>(i);
      break;
    }
  }
  if (id == UINT8_MAX) return retVal;
  for (int i = 0; i < nodeCnt; ++i) {
    if (nodes[i].tID == id) [[unlikely]] {
      retVal[index++] = nodes[i].id;
      if (index >= size) break;
    }
  }
  return retVal;
}
inline std::vector<NodeID> RnImport::getNodes(const char* name) const {
  std::vector<NodeID> retVal;
  if (name == nullptr) return retVal;
  TemplateID id = UINT8_MAX;
  for (int i = 0; i < templateCnt; ++i) {
    if (templates[i].isNodeName(fileData, name)) [[unlikely]] {
      id = static_cast<uint8_t>(i);
      break;
    }
  }
  if (id == UINT8_MAX) return retVal;
  for (int i = 0; i < nodeCnt; ++i) {
    if (nodes[i].tID == id) [[unlikely]] { retVal.push_back(nodes[i].id); }
  }
  return retVal;
}

inline std::string RnImport::getNodeName(NodeID node) const {

  if (node >= nodeCnt) [[unlikely]]
    return {};
  const auto* nData = getNodeData(node);
  if (nData == nullptr) [[unlikely]] { return {}; }
  const auto* nTemplate = getNodeTemplate(nData->tID);
  if (nTemplate == nullptr) [[unlikely]] { return {}; }

  // This way we can easily return a non allocated string aswell
  return nTemplate->getName(fileData).getString();
}

}  // namespace raynodes
#endif  //IMPORTER_H