# sc-logger

The sc-logger is a small library that consists of two parts: scf and scl.

## scf

Self-check-format (abbreviated as scf) is a library provides printf-like formatting with the following features:
- check if the input arguments match the format specifiers and if it isn’t, the compiler is required to issue an error message and stop the compiling process
- calculate indexes of the specifiers at compile time to unload runtime formatting (available non-constexpr arguments)

### Syntax

**SCFormat(format-string, arg1, arg2, ... , argN)**

The format-string must be a string literal that contains specifiers (subsequences beginning with %, like `%spec`).
The specifiers will be replaced by strings resulting from the formatting of the given arguments.

Allowed specifiers:

Specifier | Meaning | Available argument types
--- | --- | --- |
s | string | std::string, std::string_view, char *
d | integer number | [signed / unsigned] short, int, long, long long
x | hex number | [signed / unsigned] char, short, int, long, long long
c | char | [signed / unsigned] char
b | bool | bool
f | Floating-point number | float, double, long double
U | User type | any type for which an ToString() function is defined

### Examples

```c++
#include <scf/scf.h>
```

- Simple formatting:

```
const std::string res = SCFormat("%s - string, %d - int, %b - bool", "str", 1, true);
assert(res == "str - string, 1 - int, true - bool");
```

- Get compile error:

```
const std::string _ = SCFormat("%s", 1);
// error: static assertion failed: the arguments don't match the specifiers
```

- User type

Declare the UserType structure and implement ToString() function for the type:

```
struct UserType { int data = 0; };
std::string ToString(const UserType &val) { return "{" + std::to_string(val.data) + "}"; }
```

Put the UserType object to the formatter:

```
const std::string res = SCFormat("%U - user type", UserType {1});
assert(res == "{1} - user type")
```
