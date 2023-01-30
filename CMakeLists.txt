include(CMakePrintHelpers)

cmake_minimum_required(VERSION 3.10)
project(chip8)

include_directories(include)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB RECURSE INCLUDE_DIRECTORIES include/*.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} SDL2)

cmake_print_properties(TARGETS ${PROJECT_NAME} PROPERTIES LOCATION INCLUDE_DIRECTORIES)
