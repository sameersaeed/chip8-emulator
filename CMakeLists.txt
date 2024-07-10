cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

if(NOT EMSCRIPTEN)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

set(SOURCE_FILES 
    src/gui.cpp
    src/chip8.cpp
)

file(GLOB_RECURSE HEADER_FILES include/*.hpp)
include_directories(${PROJECT_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN_FILE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set(MAIN_FILE src/emscripten_main.cpp)
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${MAIN_FILE} ${HEADER_FILES})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} "-o ${CMAKE_CURRENT_LIST_DIR}/client/main.html")
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    set(MAIN_FILE src/main.cpp)
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${MAIN_FILE} ${HEADER_FILES})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

    # test executable
    add_executable(chip8_test tests/chip8_test.cpp src/chip8.cpp include/chip8.hpp)
    target_link_libraries(chip8_test GTest::gtest_main)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(chip8_test DISCOVERY_MODE PRE_TEST)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
