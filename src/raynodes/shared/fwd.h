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

#ifndef RAYNODES_SRC_SHARED_FWD_H_
#define RAYNODES_SRC_SHARED_FWD_H_

#include <cstdint>
#include "shared/defines.h"

enum PinType : uint8_t;     // Datatype of connection pins
enum MOperation : uint8_t;  // Type of math operation
enum Direction : bool;      // Which directiont the pin is factin (in/out)
enum NodeID : uint16_t;     // Unique NodeID counting up - Limits to 65536 nodes!
struct Connection;          // A connection between two components
struct Component;           // Base class for all components
struct Pin;                 // Base class for both pin types
struct InputPin;            // InputPin specialization
struct OutputPin;           // OutputPin specialization
struct Node;                // Base class for node
struct Action;              // Base class for any editor action (anything able to be undone/redone)
struct TextAction;          // Special action that represents a text change
struct NodeMovedAction;     // Special action that represents node movement
struct EditorContext;       // Core data holder for the editor
struct RaynodesPluginI;     // Base class for the plugin interface
struct NodeTemplate;        // Template to create a node (list of component names)
struct ComponentRegister;   // Interface that encaspulate registering components
struct NodeRegister;        // Interface that encaspulate registering nodes
struct EditorContext;       // Central backend data holder
struct PluginContainer;     // Holds the dll instance and the plugin instance
class Window;               // UI class
struct TextField;           // UI class
struct Vec2;                // Vector2 replacement
struct ComponentTemplate;   // Building plan for a component

using ComponentCreateFunc = Component* (*)(ComponentTemplate);        // Takes a name and returns a new Component
using NodeCreateFunc = Node* (*)(const NodeTemplate&, Vec2, NodeID);  // Creates a new node
using NodeFunc = void (*)(EditorContext&, Node*);                     // Generic function called on a node

//Raylib types
struct Color;
struct Rectangle;
struct Font;
struct Vector2;

#include "shared/types.h"

#endif  //RAYNODES_SRC_SHARED_FWD_H_