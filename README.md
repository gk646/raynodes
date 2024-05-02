# raynodes

`raynodes` is a standalone 2D node editor made using [raylib](https://github.com/raysan5/raylib) with a focus
extensibility. It aims to be an attractive tool for any node based task, but focuses on source code integration within
bigger projects like games, editors...

A small showcase of its major features:

- **Clean**, **modularized** and **documented** source code
- **Fast** and **optimized** import and export functionalities without dependencies!
- Cross-platform support
- Documented **plugin interface** and capabilities
- **User created** node templates at runtime
- Easily **extendable** with **event-driven** Component interface

In a lot of places it uses my (header only) C++ helper library [cxstructs](https://github.com/gk646/cxstructs).  
For more infos on the design choices go to [Software Design](#Software-Design)

**1.** [Controls](#Controls)   
**2.** [Editor Features](#Editor-Features)   
**3.** [Components!](#Expand-and-Contribute-with-Components)  
**4.** [Plugins](#Plugins)  
**5.** [Software Design](#Software-Design)

### Controls

- `CTRL+C` / **Copy selection**
- `CTRL+V` / **Paste selection**
- `CTRL+X` / **Cut (delete and copy) selection**
- `CTRL+Z` / **Undo Action**
- `CTRL+Y` / **Redo Action**
- `CTRL+LMB`(click on a node) / **Add to Selection**
- `CTRL+RMB`(drag) / **Delete nodes in selection**
- `RMB`  / **Open context menu**
- `RMB` (drag mouse) / **Select in rectangle**
- `DEL` / **Delete selected nodes**

CTRL = Control  
LMB = Left Mouse Button   
RMB = Right Mouse Button

## Editor Features:

### Action History

Each action (move, edit, delete + many more) are tracked in a global list improving the workflow.
Actions have a generic interface and new ones can easily be added.

### User Defined Templates *(soon)*

...

## Expand and Contribute with Components!

Add your own Components to enable users to create nodes with them.
A component is a simple struct that is drawn and update inside the node.
Inside the update function you have full write and read access to the context and design and draw complex components.

### Capabilities

The capabilities are almost endless.
Some powerful features include:

- Access other nodes and their components from your component
    - e.g. modify all nodes with a given label
- Modify editor properties like resolution or camera zoom and position
- Trigger functions like saving or adding and removing nodes

Some ides that are possible:

- Load a picture from a given path and display it
    - Take a Vector3 input and shift the colors
- Computer logic gates with clock cycle
    - Use the boolean type to activate
    - Use the integer type to pass bit data

### One Rule to rule them all

**All components are expected to stay within their bounds *(draw and update)*. Although they can freely change their
dimensions...**

### Interface

```cpp
  //-----------CORE-----------//
  //Necessary to copy the component
  virtual Component* clone() = 0;
  //IMPORTANT: Only called when its bounds are visible on the screen!
  virtual void draw(EditorContext& ec, Node& parent) = 0;
  //Guaranteed to be called once per tick (on the main thread) (not just when focused)
  virtual void update(EditorContext& ec, Node& parent) = 0;
  //Use the symmetric helpers : cx_save(file,myFloat)...
  virtual void save(FILE* file) = 0;
  //Use the symmetric helpers : cx_load(file,myFloat)...
  virtual void load(FILE* file) = 0;

  //-----------EVENTS-----------//
  // All called once, guaranteed before update() is called
  virtual void onMouseEnter(EditorContext& ec) {}
  virtual void onMouseLeave(EditorContext& ec) {}
  virtual void onFocusGain(EditorContext& ec) {}
  virtual void onFocusLoss(EditorContext& ec) {}

  //-----------LIFE CYCLE-----------//
  //Called once at creation time after the constructor
  virtual void onCreate(EditorContext& ec, Node& parent) {}
  //IMPORTANT: Only called once when the node is finally destroyed (only happens after its delete action is destroyed)
  //This may happen very delayed or even never!
  virtual void onDestruction(Node& parent) {}
  //Called whenever component is removed from the screen (delete/cut)
  virtual void onRemovedFromScreen(EditorContext& ec, Node& parent) {}
  //Called whenever component is added to the screen (paste)
  virtual void onAddedToScreen(EditorContext& ec, Node& parent) {}

  //-----------CONNECTIONS-----------//
  //Called once when a new connection is added
  virtual void onConnectionAdded(EditorContext& ec, const Connection& con) {}
  //Called once when an existing connection is removed
  virtual void onConnectionRemoved(EditorContext& ec, const Connection& con) {}
```

The interface is quite expansive and abstract allowing an implementation to react to a multitude of events.

### Loading and saving

Loading and saving its state is made easy through a supplied symmetric save format.
Just call the `io_save` which supports int, float, string and raw data(future).

This is how you save and retrieve a string:

```cpp
void TextInputField::save(FILE* file) {
cxstructs::io_save(file, buffer.c_str());
//More possible member variables:
//cxstructs::io_save(file, myFloat);
//cxstructs::io_save(file, myInt);
//cxstructs::io_save(file, myAllocatedDataPtr, mySize);
}
void TextInputField::load(FILE* file) {
cxstructs::io_load(file, buffer);
//More possible member variables
//cxstructs::io_load(file, myFloat);
//cxstructs::io_load(file, myInt);
//cxstructs::io_load(file, myAllocatedDataPtr, mySize);
}
```

## Plugins

`raynodes` provides an easy-to-use plugin interface that allows defining custom components and building nodes out of
them.

```cpp
//The raynodes plugin interface
struct RaynodesPluginI {
const char* name = nullptr;  //Will be set automatically at runtime (to the file name, for clarity)
virtual ~RaynodesPluginI() = default;
// Called once after its been loaded
virtual void onLoad(EditorContext& ec) {}
// Called once at the correct time to register its components
virtual void registerComponents(ComponentRegister& cr) {}
// Called once at the correct time to register its nodes
virtual void registerNodes(NodeRegister& nr) {}
};

extern "C" EXPORT inline RaynodesPluginI* CreatePlugin() {
 return new MyPlugin();
}
```

Implement this interface and **add the exported function to create an instance.**

### Registering Components

Components are easily registered by providing a name and its type. This works cause all Components have a similar
constructor.
After you implemented a new Component by creating a derived class in your plugin files just include its header and
register it in your plugin.

**NOTE: Components are uniquely identified by this name (naming collision will be shown when loading).**
This means it's advised to pick a namespace for your plugin (small prefix).

Syntax: `registerComponent<ComponentType>("<Component-Identifier>")`

```cpp
void BuiltIns::registerComponents(ComponentRegister& cr) {
cr.registerComponent<MathC>("MathOp");
cr.registerComponent<DisplayC>("Display");
cr.registerComponent<StringToNumberC>("StrToNum");
cr.registerComponent<TextInputField>("TextInput");
}
```

### Registering Nodes

A node is then just a container for components. The register process allows to set custom labels for each component
which helps to reference a specific component later on:

Syntax: `registerNode("<NodeName>", { {<CustomLabel>,<Component-Identifier>}, ... }`

```cpp
void BuiltIns::registerNodes(NodeRegister& nr) {
  nr.registerNode("MathOp", {{"Operation", "MathOp"}});
  nr.registerNode("TextField", {{"TextField", "TextInput"}});
  nr.registerNode("StringToNum", {{"Converter", "StrToNum"}});
  nr.registerNode("Display", {{"Display", "Display"}});
  nr.registerNode("NumberField", {{"Number", "NumberInput"}});
}
```

### Misc

**When creating your plugin you might have to link against the base editor (statically) and against raylib (shared).**

Plugins will be loaded from the `./plugins` folder relative to the executable

## Software Design

### Software General

- Prefer fixed size containers for lower level structs for better cache efficiency
    - Also helps to reduce allocations
- No global state to allow for modularity if needed (and testability...)
- Move away from providing source code extensibility in favour of in-editor extensibility
    - Project isn't centered around code interaction but in-editor interaction
- ~~2 thread model - Logic and draw thread (remove logic time overhead and memory management from draw thread)~~
    - ~~Only minimal synchronization needed (with spin lock and only in EditorContext getters and setters)~~
    - ~~Through clean code structure critical sections can easily be defined~~
- 1 Thread is enough - 2 Threads introduce code complexity
    - Logic overhead is very small
- Structure code by exporting big functions into headers

### Software Concepts

EditorContext - DataBackend

- Should just hold data and perform up to clever getter and setter tasks
    - Exceptions to this are backend tasks which have a limited scope inside the EditorContext

NodeEditor - Logic

- Will be passed a EditorContext& to operate on
- Defines logic that works on the data

### Format and Style

- General style guide [cxutil/cxtips.h](https://github.com/gk646/cxstructs/blob/master/src/cxutil/cxtips.h)

### Node Editor Concepts

The main inspiration was to create a editor that just provides a basic interface that can be used to create anything
node based like:

- Blender Node Editor
- Quest Tree (for games, DialogueTree and choices)
- Basic Logic Circuit

`raynodes` is a component-centric editor meaning that most things happen in and around components.
In turn nodes are just glorified component containers that you can move around.

This choice was made due to a number of reasons:

- Having reusable building blocks (components) throughout nodes is good
- With only a node it's not clear who handles the input and outputs
    - Now each node has its dedicated input and outputs -> might add node level connections aswell
- Components can be tied to some identifier (a string here) which allows nodes to built at runtime
    - This allows for instructions in string form on how to build a specific node -> plugins

However this also has some drawbacks:

- A node doesnt have the functionality of all its components combined
    - Its just a container... -> this could be improved with inter-component data transfers
- Having a nested structure adds extra complexity to reading and saving