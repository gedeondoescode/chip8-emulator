cmake_minimum_required(VERSION 3.10.0)
project(Chip8 VERSION 0.1.0 LANGUAGES C CXX)
find_package(SDL2 REQUIRED)
add_executable(Chip8 src/Main.cpp src/Chip8.cpp)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2)


