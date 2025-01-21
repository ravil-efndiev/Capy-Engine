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
add_executable(vk 
    "src/main.cpp"
    "src/Application.cpp"
    "src/Application.h"
    "src/include.h"
    "src/Utils.cpp"
    "src/Utils.h"
 "src/Vulkan/VulkanContext.cpp"
"src/Vulkan/VulkanContext.h" "src/Vulkan/Window.cpp" "src/Vulkan/Window.h" "src/Vulkan/Device.cpp" "src/Vulkan/Device.h" "src/Vulkan/Swapchain.h" "src/Vulkan/Swapchain.cpp" "src/Graphics/Shader.h" "src/Graphics/Shader.cpp" "src/Graphics/Pipeline.h" "src/Graphics/Pipeline.cpp" "src/Graphics/RenderPass.h" "src/Graphics/RenderPass.cpp" "src/Graphics/Framebuffer.h" "src/Graphics/Framebuffer.cpp" "src/Graphics/Renderer.h" "src/Graphics/Renderer.cpp" "src/Events/EventHandler.cpp" "src/Events/Event.h" "src/Graphics/VertexBuffer.cpp"  "src/Graphics/VertexBuffer.h" "src/Graphics/Vertex.h")

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

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
add_custom_command(
    TARGET vk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${ASSET_DIR}"
    "${EXECUTABLE_OUTPUT_PATH}/assets"
)
