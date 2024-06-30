cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES 
    src/gui.cpp
    src/chip8.cpp
)

file(GLOB_RECURSE HEADER_FILES include/*.hpp)
include_directories(${PROJECT_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN_FILE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set(MAIN_FILE src/emscriptenMain.cpp)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} "-o ${CMAKE_CURRENT_LIST_DIR}/client/main.html")

else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    set(MAIN_FILE src/main.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${MAIN_FILE} ${HEADER_FILES})

if(NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
endif()