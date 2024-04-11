# raynodes

`raynodes` is a 2D node editor meant to be easily expendable both in code and in the ui.

In a lot of places it uses my C++ helper library [cxstructs](https://github.com/gk646/cxstructs).

**1.** [Controls](#Controls)   
**2.** [Features](#Features)   
**3.** [Components!](#Expand-and-Contribute-with-Components)  
**4.** [Software Design](#Software-Design)

### Controls

- `CTRL+Z` / **Undo Action**
- `CTRL+Y` / **Redo Action**
- `CTRL+LMB`(click on a node) / **Add to Selection**
- `RMB`  / **Open context menu**
- `RMB` (drag mouse) / **Select in rectangle**
- `DEL` / **Delete selected nodes**

CTRL = Control  
LMB = Left Mouse Button   
RMB = Right Mouse Button

## Features:

### Action History

Each action (move, edit, delete + many more) are tracked in a global list improving the workflow.
Actions have a generic interface and new ones can easily be added.

### Define your own nodes *(soon)*

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
- Trigger function like saving or adding and removing nodes

### One Rule to rule them all

**All components are expected to stay within their bounds *(draw and update)*. Although they can freely change their
dimensions...**

### Interface

```cpp
struct Component {
const char* name;
uint16_t width;
uint16_t height;
explicit Component(const char* name, uint16_t w = 0, uint16_t h = 0) : name(name), width(w), height(h) {}
virtual Component* clone() = 0;
virtual void draw(float x, float y, EditorContext&) = 0;
virtual void update(float x, float y,EditorContext&) = 0;
virtual void save(FILE* file) = 0;
virtual void load(FILE* file) = 0;

//Getters
virtual const char* getString() { return nullptr; };
virtual int getInt() { return 0; };
virtual float getFloat() { return 0.0F; };
virtual void* getData() { return nullptr; };

[[nodiscard]] inline float getWidth() const { return width; }
[[nodiscard]] inline float getHeight() const { return height; }
};
```

*This interface will be expanded with some event functions the component can react to*
It has a label:`name`, a `width` and `height`;

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

## Software Design

- No global state to allow for modularity if needed (and testability...)
- Move away from providing source code extensibility in favour of in-editor extensibility
    - Project isn't centered around code interaction but in-editor interaction
- 2 thread model - Logic and draw thread (remove logic time overhead and memory management from draw thread)
    - Only minimal synchronization needed (with spin lock and only in EditorContext getters and setters)
    - Through clean code structure critical sections can easily be defined
    - Logic overhead is very small (might not be needed)
- Structured code through exporting big functions into headers

### Concepts

EditorContext - DataBackend

- Should just hold data and perform up to clever getter and setter tasks
    - Exceptions to this are backend tasks which have a limited scope inside the EditorContext

NodeEditor - Logic

- Will be passed a EditorContext& to operate on
- Defines logic that works on the data