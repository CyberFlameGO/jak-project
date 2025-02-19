# Top Level CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
project(jak)
include(CTest)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

option(BUILD_FOR_RELEASE "Build for release purposes (static everything)" OFF)
if(BUILD_FOR_RELEASE)
    set(BUILD_SHARED_LIBS OFF)
else()
    set(BUILD_SHARED_LIBS ON)
endif()

# Set default compile flags
# optimization level can be set here. You can overwrite these in a per-project basis if you want.
if(MSVC AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    message(STATUS "Clang on MSVC detected! Adding compile flags")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -Xclang -fcxx-exceptions \
        -Xclang -fexceptions \
        -Xclang -std=c++17 \
        -Xclang -D_CRT_SECURE_NO_WARNINGS \
        /arch:AVX \
        -Wno-c++11-narrowing -Wno-c++98-compat -Wno-c++20-compat -W3")

    # linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16000000,16384")

    # additional c++ and linker flags for release mode for our projects
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2")
    elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    endif()

elseif(UNIX)
    message(STATUS "GCC detected! Adding compile flags")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -Wall \
        -Winit-self \
        -ggdb \
        -Wextra \
        -Wno-cast-align \
        -Wcast-qual \
        -Wdisabled-optimization \
        -Wformat \
        -Wmissing-include-dirs \
        -Woverloaded-virtual \
        -Wredundant-decls \
        -Wshadow \
        -Wsign-promo \
        -fdiagnostics-color=always \
        -mavx")

    # additional c++ flags for release mode for our projects
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()

elseif(MSVC)
    message(STATUS "MSVC detected!! Adding compile flags")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
      # This actually breaks some standard library things for some reason?
      # message("Setting Flags to Enable Edit and Continue")
      # set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
    endif()
    # c++ flags for all build types
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8 /arch:AVX")

    # linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16000000,16384")
    
    # additional c++ and linker flags for specific build types
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2")
    elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    endif()
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
    set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 7.1.7600.0.30514) # win7.1, supports xp
    message("Windows SDK version: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

if(ASAN_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1")
    message(STATUS "Doing ASAN build")
endif()

option(CODE_COVERAGE "Enable Code Coverage Compiler Flags" OFF)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/third-party/cmake/modules/)

if(UNIX AND CODE_COVERAGE)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  message("Code Coverage build is enabled!")
else()
  message("Code Coverage build is disabled!")
endif()

# includes relative to top level jak-project folder
include_directories(./)

include_directories(SYSTEM third-party/inja)

# build repl library
add_subdirectory(third-party/replxx EXCLUDE_FROM_ALL)

# build common library
add_subdirectory(common)

# build decompiler
add_subdirectory(decompiler)

add_subdirectory(third-party/cubeb)

# build glfw library
add_subdirectory(third-party/glfw)
add_subdirectory(third-party/zstd)

# build imgui
include_directories(third-party/glad/include)
include_directories(third-party/glfw/include)
add_subdirectory(third-party/imgui)

# build the game code in C++
add_subdirectory(game)

# build the compiler
add_subdirectory(goalc)

# build standalone tools
add_subdirectory(tools)

# build the gtest libraries
if(WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(third-party/googletest EXCLUDE_FROM_ALL)
include(GoogleTest)

# build tests
include(test/CMakeLists.txt)

# build lzokay library
add_subdirectory(third-party/lzokay EXCLUDE_FROM_ALL)

# build format library
add_subdirectory(third-party/fmt EXCLUDE_FROM_ALL)
# discord rich presence
include_directories(third-party/discord-rpc/include)
add_subdirectory(third-party/discord-rpc EXCLUDE_FROM_ALL)

# build zydis third party library for disassembling x86
# NOTE: Once under CMake 3.13's policy CMP0077, override with `set()` instead
option(ZYDIS_BUILD_TOOLS "Zydis: Build tools" OFF)
option(ZYDIS_BUILD_EXAMPLES "Zydis: Build examples" OFF)
if(BUILD_SHARED_LIBS)
    option(ZYDIS_BUILD_SHARED_LIB "Zydis: Build shared library" ON)
else()
    option(ZYDIS_BUILD_SHARED_LIB "Zydis: Build shared library" OFF)
endif()
add_subdirectory(third-party/zydis EXCLUDE_FROM_ALL)

# windows memory management lib
if(WIN32)
    add_subdirectory(third-party/mman)
endif()

