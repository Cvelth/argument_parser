<!---
[![Actions Status](https://github.com/TheLartians/ModernCppStarter/workflows/MacOS/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/TheLartians/ModernCppStarter/workflows/Windows/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/TheLartians/ModernCppStarter/workflows/Ubuntu/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/TheLartians/ModernCppStarter/workflows/Style/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/TheLartians/ModernCppStarter/workflows/Install/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![codecov](https://codecov.io/gh/TheLartians/ModernCppStarter/branch/master/graph/badge.svg)](https://codecov.io/gh/TheLartians/ModernCppStarter)
-->

# C++ Project Starter

## TODO:
- Replace all occurrences of "Greeter" in the relevant CMakeLists.txt with the name of your project
- Add [your project's codecov token](https://docs.codecov.io/docs/quick-start) to your project's github secrets under `CODECOV_TOKEN`
- Happy coding!
- Don't forget to write a README for you new project. 
- Feel free to replace the License with one suited for your project.

## Usage

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -Hstandalone -Bbuild/standalone
cmake --build build/standalone
./build/standalone/Greeter --help
```

### Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -Htest -Bbuild/test
cmake --build build/test
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test --target test

# or simply call the executable: 
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

### Additional tools

The project includes an [tools.cmake](cmake/tools.cmake) file that can be used to import additional tools on-demand through CMake configuration arguments.
The following are currently supported.

- `-DUSE_SANITIZER=<Address | Memory | MemoryWithOrigins | Undefined | Thread | Leak | 'Address;Undefined'>`
- `-DUSE_CCACHE=<YES | NO>`
