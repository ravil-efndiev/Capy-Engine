set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOME $ENV{USERPROFILE})
set(CMAKE_TOOLCHAIN_FILE ${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

add_executable(vk 
    "src/main.cpp"
    "src/Application.cpp"
    "src/Application.hpp"
    "src/include.hpp"
    "src/Utils.cpp"
    "src/Utils.hpp"
)

target_include_directories(vk PUBLIC
    src/
)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(vk PRIVATE 
    glfw 
    glm::glm-header-only 
    Vulkan::Vulkan
)
