cmake_minimum_required(VERSION 3.2)

project(App)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOME $ENV{USERPROFILE})

file(GLOB_RECURSE APP_SOURCES "src/*.cpp" "src/*.h")
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${APP_SOURCES})

add_executable(App ${APP_SOURCES})

target_link_libraries(App PRIVATE Capy)

target_include_directories(App PRIVATE ../CapyEngine/src)

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/CapyEngine/assets")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/App/")
add_custom_command(
    TARGET App POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${ASSET_DIR}"
    "${EXECUTABLE_OUTPUT_PATH}/assets"
)
