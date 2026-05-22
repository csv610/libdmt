cmake_minimum_required(VERSION 3.10)
project(DiscreteMorseTheory VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Executables for examples
add_executable(example_2d examples/example_2d.cpp)
add_executable(example_3d examples/example_3d.cpp)
add_executable(example_surface examples/example_surface.cpp)
add_executable(example_dmf examples/example_dmf.cpp)

# Unit tests
add_executable(unit_tests tests/test_dmt.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
