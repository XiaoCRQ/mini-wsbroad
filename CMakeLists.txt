cmake_minimum_required(VERSION 3.14)
project(mini-wsbroad VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制要求静态链接 (针对 Linux 兼容性)
if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    # 如果想更彻底地静态化，可以使用 set(BUILD_SHARED_LIBS OFF)
endif()

include(FetchContent)

# 1. nlohmann/json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# 2. Asio (Standalone)
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz)
FetchContent_MakeAvailable(asio)

# 3. WebSocket++
FetchContent_Declare(websocketpp GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git GIT_TAG master)
FetchContent_MakeAvailable(websocketpp)

add_executable(mini-wsbroad main.cpp)

# 解决 Windows 报错的关键：显式定义预处理器宏，防止它去找 boost
target_compile_definitions(mini-wsbroad PRIVATE 
    ASIO_STANDALONE 
    _WEBSOCKETPP_CPP11_STANDALONE_ASIO
)

target_include_directories(mini-wsbroad PRIVATE 
    ${websocketpp_SOURCE_DIR}
    ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(mini-wsbroad PRIVATE nlohmann_json::nlohmann_json)

find_package(Threads REQUIRED)
target_link_libraries(mini-wsbroad PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(mini-wsbroad PRIVATE ws2_32 mswsock)
    # 移除源码中重复的宏定义警告
    target_compile_options(mini-wsbroad PRIVATE /wd4005) 
endif()
