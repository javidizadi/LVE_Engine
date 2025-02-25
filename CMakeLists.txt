cmake_minimum_required(VERSION 3.10)
project(LVE_Engine)

# Set C++ standard and build type
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find dependencies
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

# Main executable
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(LVE_Engine ${SOURCES})

# Include directories
target_include_directories(LVE_Engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(LVE_Engine PRIVATE
    glfw
    Vulkan::Vulkan
    wayland-client
)

# Test configure
enable_testing()
find_package(doctest REQUIRED)
add_executable(watchdog_test tests/watchdog_test.cpp src/watchdog.cpp)
add_test(NAME watchdog COMMAND watchdog_test)

# Debug flags
target_compile_options(LVE_Engine PRIVATE
    $<$<CONFIG:Debug>:-ggdb>
)
