cmake_minimum_required(VERSION 3.14)
project(mini-wsbroad VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. 下载 nlohmann/json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# 2. 下载 websocketpp (Header-only)
FetchContent_Declare(websocketpp GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git GIT_TAG master)
FetchContent_MakeAvailable(websocketpp)

# 3. 下载 Asio (解决 asio/version.hpp 找不到的问题)
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz)
FetchContent_MakeAvailable(asio)

add_executable(mini-wsbroad main.cpp)

# 关联所有头文件和库
target_link_libraries(mini-wsbroad PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(mini-wsbroad PRIVATE 
    ${websocketpp_SOURCE_DIR}
    ${asio_SOURCE_DIR}/asio/include  # 关键点：手动指向 asio 内部的 include 目录
)

# 定义宏 (对应你代码开头的定义)
target_compile_definitions(mini-wsbroad PRIVATE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STANDALONE_ASIO)

# 线程支持
find_package(Threads REQUIRED)
target_link_libraries(mini-wsbroad PRIVATE Threads::Threads)

# Windows 下的网络库链接
if(WIN32)
    target_link_libraries(mini-wsbroad PRIVATE ws2_32 mswsock)
endif()
