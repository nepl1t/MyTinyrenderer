cmake_minimum_required(VERSION 3.12...3.26)

# 把默认构建类型设为 Debug（开发用），可改回 Release
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug default for development")
endif()

project(tinyrenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(iwyu "Run include-what-you-use")
if(iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXE})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
  add_compile_options(-Wall)
endif()

find_package(OpenMP COMPONENTS CXX)

set(SOURCES main.cpp tgaimage.cpp model.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
# 为本地仓库中的 Eigen 头文件添加 include 路径（Eigen 为 header-only）
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/libs/eigen-3.4.1")

target_link_libraries(${PROJECT_NAME} PRIVATE $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)

# 为可执行目标在 Debug 配置下添加 -g -O0（非多配置生成器也适用）
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Debug>:-Wall>
)

file(GENERATE OUTPUT .gitignore CONTENT "*")
