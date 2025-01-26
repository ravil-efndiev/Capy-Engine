cmake_minimum_required(VERSION 3.2)

project(Capy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOME $ENV{USERPROFILE})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})

add_library(Capy STATIC ${SOURCES})

target_include_directories(Capy PUBLIC src/)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(Capy PUBLIC 
    glfw 
    glm::glm-header-only 
    Vulkan::Vulkan
)
