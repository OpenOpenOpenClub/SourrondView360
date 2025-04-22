# filepath: /home/wind/Projects/AdasSourrondView/CMakeLists.txt
cmake_minimum_required(VERSION 3.8) # 提高版本要求以支持更好的 FetchContent 或 find_package

project(avm_app_3d LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17) # 建议使用 C++17 或更高版本
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED) # 用于查找 OpenGL 库
find_package(glfw3 REQUIRED) # 用于查找 GLFW 库
find_package(glm REQUIRED) # 如果使用系统安装的 GLM

# GLAD (需要预先生成准备好)
add_library(glad_lib ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c)
target_include_directories(glad_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include)

# --- 包含目录 ---
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src # 顶层 srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imaging
    ${glm_SOURCE_DIR} # GLM include directory from FetchContent
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include # GLAD include directory
)

message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "GLFW include path: ${glfw3_INCLUDE_DIRS}")
message(STATUS "GLM include path: ${glm_SOURCE_DIR}") # For FetchContent

# --- 源文件 ---
# 将 avm_app_demo.cpp 移动到 srcs/app/main.cpp (或者创建一个新的)
# 将 common.cpp 移动到 srcs/common/common.cpp
# 将 common.h 移动到 srcs/common/common.h
# 将 prms.hpp 移动到 srcs/common/prms.hpp
add_executable(avm_app_3d
    src/app/main.cpp # 新的主文件
    src/common/common.cpp

    src/rendering/shader.cpp # <--- 添加 Shader 源文件
    src/rendering/renderer.cpp # <--- 添加 Renderer 源文件

    # src/rendering/renderer.cpp # 稍后添加
    # src/scene/scenemanager.cpp # 稍后添加
    # src/imaging/imageprocessor.cpp # 稍后添加
    # src/rendering/cameracontroller.cpp # 稍后添加
    # src/glad.c # 如果手动包含 GLAD 源文件
)

# --- 链接库 ---
target_link_libraries(avm_app_3d
    PRIVATE
    ${OpenCV_LIBS}
    OpenGL::GL # 标准 OpenGL 目标
    glfw # GLFW 目标
    glad_lib # 如果创建了 glad_lib 目标
)

# # --- 保留旧的标定程序 (如果需要) ---
# add_executable(avm_cali avm_cali_demo.cpp src/common/common.cpp)
# target_link_libraries(avm_cali PRIVATE ${OpenCV_LIBS})
