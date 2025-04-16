
cmake_minimum_required(VERSION 3.10)
project(mmm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_BUILD_TYPE Debug)#设置调试信息

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")

add_executable(mmm ${SOURCES} ${HEADERS})

#target_link_libraries(mmm, pthread)#链接一下多线程库