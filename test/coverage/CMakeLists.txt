cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(argument_parser_coverage_tests
  LANGUAGES CXX
)

# ---- Dependencies ----

include(../../cmake/CPM.cmake)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.7
)

# ---- Create binary ----

FILE(GLOB_RECURSE include_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.{h,hpp}")
FILE(GLOB_RECURSE test_include_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.{h,hpp}")
FILE(GLOB_RECURSE test_source_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../source/*.{h,hpp,cpp}")

add_custom_command(
  OUTPUT argument_parser.cpp
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/to_single_cpp.py -i ${CMAKE_CURRENT_SOURCE_DIR}/../../include -s ${CMAKE_CURRENT_SOURCE_DIR}/../source -f argument_parser -o .
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/to_single_cpp.py ${include_files} ${test_include_files} ${test_source_files}  
  COMMENT "Creating single cpp file."
)
add_executable(argument_parser_coverage_tests argument_parser.cpp)

target_link_libraries(argument_parser_coverage_tests doctest)
set_target_properties(argument_parser_coverage_tests PROPERTIES CXX_STANDARD 20)
target_compile_definitions(argument_parser_coverage_tests PUBLIC DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

target_compile_options(argument_parser_coverage_tests PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
target_link_options(argument_parser_coverage_tests PUBLIC -fprofile-arcs -ftest-coverage)

# enable compiler warnings
if (NOT TEST_INSTALLED_VERSION)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(argument_parser_coverage_tests PUBLIC -Wall -pedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(argument_parser_coverage_tests PUBLIC /W4 /WX)
    target_compile_definitions(argument_parser_coverage_tests PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add tests ----

ENABLE_TESTING() 

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(argument_parser_coverage_tests)
