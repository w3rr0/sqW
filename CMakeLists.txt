cmake_minimum_required(VERSION 3.20)
project(sqW)

add_definitions(-DGL_SILENCE_DEPRECATION)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    set(BISON_EXECUTABLE "/opt/homebrew/opt/bison/bin/bison")
    set(FLEX_EXECUTABLE "/opt/homebrew/opt/flex/bin/flex")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/bison")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/flex")
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

BISON_TARGET(MyParser grammar/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(MyScanner grammar/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include(FetchContent)

set(JUST_INSTALL_CEREAL ON CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable Cereal tests" FORCE)
set(BUILD_SANDBOX OFF CACHE BOOL "Disable Cereal sandbox" FORCE)

# Cereal
FetchContent_Declare(
    cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal.git
    GIT_TAG        v1.3.2
)
FetchContent_MakeAvailable(cereal)

# Imgui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        include/gui/Utils.hpp
)

# G Test
enable_testing()

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/backends)

file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(sqW-Core STATIC
        ${ALL_SOURCES}
        ${IMGUI_SOURCES}
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
)

target_link_libraries(sqW-Core PUBLIC
        cereal
        glfw
        ${OPENGL_LIBRARIES}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

add_executable(sqW src/main.cpp)
target_link_libraries(sqW PRIVATE sqW-Core)


add_executable(sqW-Tests ${TEST_SOURCES})
target_link_libraries(sqW-Tests PRIVATE
        sqW-Core
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(sqW-Tests)
