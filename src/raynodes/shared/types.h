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

#ifndef TYPES_H
#define TYPES_H

struct PluginContainer {
  void* handle;
  const char* name;
  RaynodesPluginI* plugin;
  void free();
};

struct Uints8 {
  uint8_t x;
  uint8_t y;
};

struct Ints {
  uint16_t a;
  uint16_t b;
};

struct Int2 {
  int x;
  int y;
};

struct Pointer {
  void* data;
  uint32_t size;
};

// Potentially use bit encoding to save more information - size doesnt need 32 bit
struct ImageData {
  void* data;
  uint32_t size;
  uint16_t x;
  uint16_t y;
};

// Dont wanna include raylib everywhere
struct Vec2 {
  float x;
  float y;
};

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Color4 {
  unsigned char r;  // Color red value
  unsigned char g;  // Color green value
  unsigned char b;  // Color blue value
  unsigned char a;  // Color alpha value
};

struct StandalonePin {
  Node* node;
  Component* component;
  Pin* pin;
};

struct ComponentTemplate {
  const char* label = nullptr;
  const char* component = nullptr;
};

struct NodeTemplate {
  const char* label = nullptr;
  Color4 color = {0, 0, 0, 255};                 // BLACK
  ComponentTemplate components[COMPS_PER_NODE];  // Current limit
};

using NodeCreateFunc = Node* (*)(const NodeTemplate&, Vec2, NodeID);  // Creates a new node

struct NodeInfo {
  NodeTemplate nTemplate{};
  NodeCreateFunc createFunc = nullptr;
};

// E = Editor
// N = Node
// C = Component
enum ColorIndex : uint8_t { E_BACK_GROUND = 0, E_GRID, N_BACK_GROUND, UI_LIGHT, UI_MEDIUM, UI_DARK, INDEX_END };

enum ScaleDirection : uint8_t { HORIZONTAL, VERTICAL };

// Flavour of components with only 1 datatype - specifies if it has both input and output or only either one
// This is mostly cosmetic - for example if you have a complex node and only want to output at node level, you can make the components themselves have no outputs
enum ComponentStyle : uint8_t { INPUT_ONLY, OUTPUT_ONLY, IN_AND_OUT };

enum WindowType : uint8_t { SETTINGS_MENU, HELP_MENU, NODE_CREATOR };

enum PluginPriority : uint8_t { ESSENTIAL, CRITICAL, HIGH, MEDIUM, LOW };

#endif  //TYPES_H