# Developer convention

- Think about the base case
    - Simple use, easily extendable
- If you need more than 3 indentation you are fucked anyway.

## Naming convention

- Modules are PascalCase
- Classes, structs are PascalCase
- Functions are snake_case
- Variables are snake_case
- Macros are upper SNAKE_CASE

## Module convention

Every module defines a macro to signal that it is included. 
If module is named Logger than the macro is MADLIBRARY_LOGGER