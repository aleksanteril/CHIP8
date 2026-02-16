cmake_minimum_required(VERSION 3.11.0)
project(CHIP8 VERSION 0.1.0 LANGUAGES C CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch SDL3 from GitHub
include(FetchContent)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG main 
)

set(SDL3_TESTS OFF CACHE BOOL "" FORCE)
set(SDL3_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

file(GLOB EMU_SRC "src/*.cpp" "src/*.hpp")
add_executable(CHIP8 ${EMU_SRC})

# Prevent WinMain duplication
target_compile_definitions(CHIP8 PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(CHIP8 PRIVATE SDL3::SDL3)
