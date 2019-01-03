# Changelog
All notable changes to this project will be documented in this file.
Refer to the Merge Request when there is one.

### MR #1 Add basic autocompletion
    basic implementation of the prompt with GNU readline
    path executables are suggested

### MR #3 Add shell parsing
    Rework of all parsing with yacc and lex to have good code
    (at least readable, its still C programming)
    Add `hashmap_get` and fix `get` function

### MR #4 Add structures
    Implementation of array, linked lists, hashset and hashmap for further work

### Execution of commands (branch exec)
    Interpretation of commands
    Implementation of builtins (mpsh-builtins.man)
    Can run a command in background

### Enhancements
    Track memory leaks
    Implements `fg` and `jobs` builtins
