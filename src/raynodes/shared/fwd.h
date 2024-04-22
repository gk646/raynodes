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

enum class NodeType : uint8_t;
enum class PinType : uint8_t;
enum MOperation : uint8_t;
enum Direction : bool;
enum ComponentType : uint8_t;
enum NodeID : uint16_t;
struct Connection;
struct Component;
struct Pin;
struct Node;
struct Action;
struct EditorContext;
struct Color;
struct Vector2;
struct Rectangle;
struct InputPin;
struct OutputPin;
struct EditorContext;
struct RaynodePluginI;

#endif  //RAYNODES_SRC_SHARED_FWD_H_