#ifndef RAYNODES_SRC_SHARED_FWD_H_
#define RAYNODES_SRC_SHARED_FWD_H_

#include <cstdint>

enum class NodeType : uint8_t;
enum class PinType : uint8_t;
enum Direction : bool;
enum ComponentType : uint8_t;
enum NodeID : uint16_t;
struct Connection;
struct ConnectionData;
struct Pin;
struct Node;
struct Action;
struct EditorContext;
struct Color;
struct Vector2;
struct Rectangle;

#endif  //RAYNODES_SRC_SHARED_FWD_H_