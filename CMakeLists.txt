cmake_minimum_required(VERSION 3.10)
project(MemoryCardConverter VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable with sources from the src directory
add_executable(convmcecc 
    src/main.cpp
    src/MemoryCardManager.cpp
    src/MemoryCardManager.h
)

# Add include directories
target_include_directories(convmcecc PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compiler warnings
if(MSVC)
    target_compile_options(convmcecc PRIVATE /W4)
else()
    target_compile_options(convmcecc PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS convmcecc DESTINATION bin)