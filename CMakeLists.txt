# 指定cmake版本
cmake_minimum_required(VERSION 3.3)
# 工程名
project(all_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -std=c++14 -fPIC -g -pthread")

# 运行测试
add_definitions(-DTEST_ENABLE)

# 添加.h文件
include_directories(src src/test)

file(GLOB_RECURSE ALL_LIBRIRY_SRCS "src/*.c*")

# 执行文件
add_executable(${PROJECT_NAME} ${ALL_LIBRIRY_SRCS})
target_link_libraries(${PROJECT_NAME})
