cmake_minimum_required(VERSION 3.15)
project(bad_apple VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build SDL2 as a static library
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
   SDL2
   GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
   GIT_TAG release-2.28.5 # we need using SDL2 instead of SDL3 because i have problem with SDL3
)

FetchContent_GetProperties(SDL2)

if (NOT SDL2_POPULATED)
   FetchContent_Populate(SDL2)
   add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
endif()

add_executable(bad_apple main.cpp)
target_include_directories(bad_apple
   PRIVATE
   ${stb_SOURCE_DIR}
   ${SDL2_SOURCE_DIR}/include
)
target_link_libraries(bad_apple PRIVATE SDL2::SDL2-static)