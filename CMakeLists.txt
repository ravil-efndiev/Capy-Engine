cmake_minimum_required(VERSION 3.2)

project(Capy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOME $ENV{USERPROFILE})

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE ${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
else()  
    set(CMAKE_TOOLCHAIN_FILE ~/tools/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "") 
    set(glm_DIR ~/tools/vcpkg/installed/x64-linux/share/glm/)
    set(glfw3_DIR ~/tools/vcpkg/installed/x64-linux/share/glfw3/)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})

add_executable(Capy ${SOURCES})

target_include_directories(Capy PUBLIC src/)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(Capy PRIVATE 
    glfw 
    glm::glm-header-only 
    Vulkan::Vulkan
)

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
add_custom_command(
    TARGET Capy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${ASSET_DIR}"
    "${EXECUTABLE_OUTPUT_PATH}/assets"
)

