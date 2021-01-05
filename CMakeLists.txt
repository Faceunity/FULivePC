cmake_minimum_required(VERSION 3.5)
# 解决 Xcode 12 cmake 构建失败的问题
if(APPLE)
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
endif()
project(FULivePC)
add_subdirectory(FULivePC)

