raynodes - Software Design


- Fully use my 1mb of stack for state data
- No global state to allow for modularity if needed (and testability...)
- Move away from providing source code extensibility to in editor extensibility
    - software isn't centered around code interaction but in editor interaction
- 2 thread model - Logic and draw thread (remove logic time overhead and memory management from draw thread)
    - only minimal synchronization needed (with spin lock and only in EditorContext getters and setters)
    - through clean code structure critical sections can easily be defined
- Structured code through exporting big functions into headers


## Concepts


EditorContext - DataBackend

- Should just hold data and perform up to clever getter and setter tasks
    - exceptions are backend tasks which have a limited scope inside the context


NodeEditor - Logic

- Will be passed a EditorContext& to operate on
- Defines logic that works on the data