# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.20.0-rc4)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(cereal-populate NONE)

# Pass through things we've already detected in the main project to avoid
# paying the cost of redetecting them again in ExternalProject_Add()
set(GIT_EXECUTABLE [==[C:/Program Files/Git/cmd/git.exe]==])
set(GIT_VERSION_STRING [==[2.30.0.windows.2]==])
set_property(GLOBAL PROPERTY _CMAKE_FindGit_GIT_EXECUTABLE_VERSION
  [==[C:/Program Files/Git/cmd/git.exe;2.30.0.windows.2]==]
)


include(ExternalProject)
ExternalProject_Add(cereal-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/USCiLab/cereal.git" "GIT_TAG" "64f50dbd5cecdaba785217e2b0aeea3a4f1cdfab" "CMAKE_ARGS" "-Wno-dev" "-DSKIP_PORTABILITY_TEST=ON" "-DSKIP_PERFORMANCE_COMPARISON=ON" "-DBUILD_TESTS=OFF"
                    SOURCE_DIR          "C:/Users/hk100/OneDrive/Documents/GitHub/hashing-schemes/external/cereal"
                    BINARY_DIR          "C:/Users/hk100/OneDrive/Documents/GitHub/hashing-schemes/build/_deps/cereal-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


