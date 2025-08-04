cmake_minimum_required(VERSION 3.15)
project(ultimate)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

set(CMAKE_C_STANDARD 99)

add_compile_definitions(RAYLIB_CUSTOM_CONFIG)
# Raylib requires OpenGL and other system libs

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)
#add_subdirectory(vendor/raylib)

include_directories(include)         # Your config.h
include_directories(vendor/raylib/src)
include_directories(vendor/raylib/src/external)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} raylib)
target_sources(ultimate PRIVATE ${SOURCE_FILES})

add_subdirectory(src)

# macOS/Linux extra libs (raylib needs these)
if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} m dl pthread GL X11 Xrandr Xinerama Xcursor Xi)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(OpenGL_LIBRARY OpenGL REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(CoreVideo_LIBRARY CoreVideo REQUIRED)

    target_link_libraries(game ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${CoreVideo_LIBRARY})
endif()
