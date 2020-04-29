cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(argument_parser_tests
  LANGUAGES CXX
)

# ---- Options ----

option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.7
)

if (TEST_INSTALLED_VERSION)
  find_package(argument_parser REQUIRED)
else()
  CPMAddPackage(
    NAME argument_parser
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
  )
endif()

CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.3
)

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(argument_parser_tests ${sources})
target_link_libraries(argument_parser_tests doctest argument_parser)

set_target_properties(argument_parser_tests PROPERTIES CXX_STANDARD 20)

# enable compiler warnings
if (NOT TEST_INSTALLED_VERSION)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(argument_parser INTERFACE -Wall -pedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(argument_parser INTERFACE /W4 /WX)
    target_compile_definitions(argument_parser INTERFACE DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add argument_parser_tests ----

ENABLE_TESTING() 

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(argument_parser_tests)
