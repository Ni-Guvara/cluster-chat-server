cmake_minimum_required(VERSION 3.0)

project(cluster-chat-server)

# 配置编译选项
set(CMAKE_CXX_CFLAGS ${CMAKE_CXX_CFLAGS} -g)

# 可执行文件的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 头文件目录搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 加载子目录
add_subdirectory(src)
