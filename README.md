[![Actions Status](https://github.com/Cvelth/argument_parser/workflows/MacOS/badge.svg)](https://github.com/Cvelth/argument_parser/actions)
[![Actions Status](https://github.com/Cvelth/argument_parser/workflows/Windows/badge.svg)](https://github.com/Cvelth/argument_parser/actions)
[![Actions Status](https://github.com/Cvelth/argument_parser/workflows/Ubuntu/badge.svg)](https://github.com/Cvelth/argument_parser/actions)
[![Actions Status](https://github.com/Cvelth/argument_parser/workflows/Style/badge.svg)](https://github.com/Cvelth/argument_parser/actions)
[![Actions Status](https://github.com/Cvelth/argument_parser/workflows/Install/badge.svg)](https://github.com/Cvelth/argument_parser/actions)
[![codecov](https://codecov.io/gh/Cvelth/argument_parser/branch/master/graph/badge.svg)](https://codecov.io/gh/Cvelth/argument_parser)

# Argument Parser
A small single header only library for command line options parsing.

## Usage

### Defining Arguments

Include the header:

```cpp
#include "argument_parser.hpp"
```

Create an `ap::arguments` object passing argument-objects to the constructor. For example:
```cpp
ap::arguments args {
    ap::flag("-help", "h"),
    ap::positional("value")
};
```

Parsing is done with:
```cpp
int main(int argc, char **argv) {
    ... 
    auto results = args.parse(argc, argv);
    ...
}
```

Additional arguments can be added before the parsing:

```cpp
args.add(
    ap::value("-optional_path", "o"),
    ap::callable([](){/* do something */}, "activate")
)
```

Attempt to add a new argument after parsing raises an error.

### Accessing Results

Method function `parse` returns an `ap::parsing_results` object.

This object has overloaded `operator bool() const` allowing to:
```cpp
if (results) {
    // on success
} else {
    // on error
}
```
Parsing errors can be accessed:
```cpp
std::vector<std::string> const &errors() const noexcept; // vector of error strings
std::vector<std::string> const &warnings() const noexcept; // vector of warning strings

std::string error_string() const; // output of errors() concatenated into a single string
std::string warning_string() const; // output of warnings() concatenated into a single string

std::string what() const; // equivalent to error_string()
std::string verbose() const; // concatenation of both errors and warning strings
```

Argument values can be accessed using `get()` method or overloaded `operator[]()`.
```cpp
ap::parsing_result::get(/*name*/) // returns std::optional<> of a result
ap::parsing_result::operator[](/*name*/) // return result, but throws an ap::access_error object on error.
```

An object returned by these methods has three data access templates:
```cpp
std::optional<T> get<T>() const; // std::nullopt, if unable to convert to T.
T as<T>() const; // throws an ap::access_error object, if unable to convert to T.
T value_or<T>(T) const; // returns passed value, if unable to convert to T. T can be deduced by the compiler
```

Example:
```cpp
args["value"].value_or(-1); // returns value passed to the "value" argument 
                            // (as int (type deduced from "-1")) if it was passed 
                            // and can be converted. "-1" otherwise.
```

### Example:
```cpp
#include "argument_parser.hpp" // header
#include <iostream> // for std::cout and std::cerr

int main(int argc, char **argv) {
    ap::arguments args {
        ap::flag("-help", "h"), // help-flag
        ap::positional("value") // positional value
    }; // define arguments
    
    auto results = args.parse(argc, argv); // parse
    
    if (results) { // if parsed successfully
        if(results["-help"]) // if <--help> was passed
            std::cout << "Carefully designed help\n";
        else 
            if(auto value = results["value"].get<std::size_t>(); 
                    value) // if value is passed and convertible to std::size_t
                foo(*value); // do something with std::size_t value
        else
            std::cout << "The value is unsupported";
    } else
        std::cerr << results.what(); // print errors
        
    return 0;
}
```

### Argument Types
#### ap::flag
The simplest argument type: it's either provided or not. 
If the name was mentioned at least once, the value is true, otherwise it's false.
Conversions to any type except for `bool` will fail.

#### ap::value
This argument type is capable of storing single value, provided in either `-value_arg value` or `-value_arg=value` form.
Usage in the form of `-value_arg -another_arg` will cause an error to be raised.
Conversions to string types (`std::string`, `std::string_view` and `const char *`) and types implicitly constructible or convertible from them are guaranteed.
Conversions to types satisfying `std::is_arithmetic` (except for `bool`) are provided using `std::from_chars` and return errors accordingly to the result of the conversion. Types implicitly convertible from these are also allowed.

#### ap::positional
This argument is equivalent to `ap::value` except it's also triggered in the cases where value is met without an active value-argument.
The order in which these arguments were added (using constructor or `ap::arguments::add`) determines order of usage.
For example:

```cpp
ap::arguments args {
    ap::positional{"-first", "f"},
    ap::positional{"s", "-second"}
}
```

After running ```application_name value_1, value_2``` next is ensured:

```cpp
results["f"].as<std::string> == "value_1"s && results["s"].as<std::string> == "value_2"s
```

#### ap::counter
This argument type is similar to `ap::flag` except it remembers how many times the argument's name(s) was mentioned.
It's convertible to bool (`true` if any of aliases was mentioned at least once) and to any type constructible from `std::size_t`.

#### ap::callable
The most customizable argument type. Constructor accepts either `std::function<void()>` or `std::function<void(std::string_view const&)>`, as well as both of them in addition to aliases.
When a value is passed into this argument, value accepting overload is called (if it was defined, error otherwise). Where there's no value, second overload is called (if it wa defined, error otherwise).
This argument type is only convertible to `bool` similarly to flag. Any value-related functionality is up to the user. Functions are called from `ap::arguments::parse()` during its processing.

## Build and run the library

No need to, it's header only. Just grab a copy of it and use in whatever projects you please.

### Build and run tests

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -Htest -Bbuild/test
cmake --build build/test
./build/test/GreeterTests
```

To collect code coverage information, run CMake with the `-DENABLE_TEST_COVERAGE=1` option.

### Run clang-format

Use the following commands from the project's root directory to run clang-format (must be installed on the host system).

```bash
cmake -Htest -Bbuild/test

# view changes
cmake --build build/test --target format

# apply changes
cmake --build build/test --target fix-format
```

See [Format.cmake](https://github.com/TheLartians/Format.cmake) for more options.
