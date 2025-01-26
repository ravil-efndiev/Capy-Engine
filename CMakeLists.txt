cmake_minimum_required(VERSION 3.2)

project(CapySolution)

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

add_subdirectory(CapyEngine)
add_subdirectory(App)


