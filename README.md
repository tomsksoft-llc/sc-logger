# sc-logger

The sc-logger is a small library that consists of two parts: scf and scl.

## scf

Self-check-format (abbreviated as scf) is a library provides printf-like formatting with the following features:
- check if the input arguments match the format specifiers and if it is not,
the compiler is required to issue an error message and stop the compiling process
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
// error: static assertion failed: the arguments do not match the specifiers
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
## scl

Self-check-logger (abbreviated as scl) is a library for CI purposes.
The library provides classes for logging actions, session info and debug messages
for Continuous Integration systems (abbreviated as CIS).

### Usage

The main class provides methods for the logging is `Logger`.
The user is responsible for creating and storing an object of this class.
The `Logger` takes a record parameters like optional session id and action and required log level
and message string, converts it to the special structure `RecordInfo`
and moves the record to an recorder.
The `Logger` constructor takes a non-moving pointer to an `IRecorder` object.

There are two standard recorders: `ConsoleRecorder` and `FileRecorder`.
First prints records to the stdout, second writes records to a specified file.
Also the user can create his own recorder via implement the `IRecorder` interface.

First create a recorder, for example `ConsoleRecorder` object:

```
scl::ConsoleRecorder::Options console_options{
    scl::AlignInfo{10 /*action_length*/, 15 /*session_id_length*/}
};
std::unique_ptr<ConsoleRecorder> console_recorder = scl::ConsoleRecorder::Init(console_options);
```

Second create a logger.
Note the `Init()` method returns either pointer to a `Logger` instance or an error.

```
std::variant<LoggerPtr, InitError> console_logger_result
    = scl::Logger::Init(options, std::move(console_recorder)));
// Note process also the InitError
LoggerPtr console_logger = std::get<LoggerPtr>(std::move(console_logger_result);
```

Third use the `SCFormat()` function to format a log message and make a record:

```
console_logger->Record(scl::Level::Info,
                       SCFormat("First record has been wrote at %d UNIX time"),
                       std::time(nullptr));
```
