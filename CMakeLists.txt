cmake_minimum_required(VERSION 3.19)
project(Hashing)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")

FetchContent_Declare(
        cereal
        GIT_REPOSITORY git@github.com:USCiLab/cereal.git
        GIT_TAG 64f50dbd5cecdaba785217e2b0aeea3a4f1cdfab
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/cereal
        CMAKE_ARGS -Wno-dev -DSKIP_PORTABILITY_TEST=ON -DSKIP_PERFORMANCE_COMPARISON=ON -DBUILD_TESTS=OFF
)

set(SKIP_PORTABILITY_TEST ON)
set(SKIP_PERFORMANCE_COMPARISON ON)
set(BUILD_TESTS OFF)
FetchContent_MakeAvailable(cereal)

add_subdirectory(src)

add_subdirectory(tests)
