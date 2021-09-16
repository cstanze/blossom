# Core Functions

Blossom contains a few core functions that are important to a developer. Below are the listed functions alongside their descriptions.

| Function  | Description                                                                                                        |
|-----------|--------------------------------------------------------------------------------------------------------------------|
| `mload`   | Load a C++ module                                                                                                  |
| `import`  | Import a Blossom module (only available in classic mode), otherwise use `import` keyword                           |
| `raise`   | Raise an error (takes one argument - a string message)                                                             |
| `print`   | Print to the console                                                                                               |
| `println` | Print to the console and append a newline. Print a newline if no arguments                                         |

## Copy function

Every type has a `copy` member function. This makes a copy of the current object. This is useful but use it wisely as it does duplicate memory, increasing usage.
