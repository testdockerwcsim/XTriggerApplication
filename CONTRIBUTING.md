# Coding conventions

Try to adhere to these coding conventions if possible. They ensure a cleanly
readable code throughout the project:

*   Do not use `auto`
*   Use two spaces for indentation
*   Use `UpperCamelCase` for class names
*   Use `UpperCamelCase` for methods
*   Use `snake_case` for variables
*   Add a `m_` prefix to member variables
*   Explain your code with helpful (!) comments, ideally using Doxygen syntax
*   Classes, functions, methods, and if statements should place the opening brace at the end of the line, rather than on a new line
    * e.g. `if(1) {`
    * not  ```
if
{
```
*   Short if statements should be put on one line
    * e.g. `if(1) return true;`
    * not  ```
if(1)
return true
```
