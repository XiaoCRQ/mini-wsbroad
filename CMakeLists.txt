cmake_minimum_required(VERSION 3.14)
project(mini-wsbroad VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制开启静态链接（针对 Linux 兼容性）
if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

include(FetchContent)

# 1. JSON
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# 2. ASIO
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz)
FetchContent_MakeAvailable(asio)

# 3. WebSocket++ (注意：为了彻底摆脱 Boost，我们需要手动处理其头文件路径)
FetchContent_Declare(websocketpp GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git GIT_TAG master)
FetchContent_MakeAvailable(websocketpp)

add_executable(mini-wsbroad main.cpp)

# 解决 Windows 重复定义警告：移除 CMake 中的定义，改用代码里的或统一处理
target_compile_definitions(mini-wsbroad PRIVATE 
    ASIO_STANDALONE 
    _WEBSOCKETPP_CPP11_STANDALONE_ASIO
)

target_include_directories(mini-wsbroad PRIVATE 
    ${websocketpp_SOURCE_DIR}
    ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(mini-wsbroad PRIVATE nlohmann_json::nlohmann_json)

# 线程
find_package(Threads REQUIRED)
target_link_libraries(mini-wsbroad PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(mini-wsbroad PRIVATE ws2_32 mswsock)
    # 强制不使用 Boost
    target_compile_definitions(mini-wsbroad PRIVATE _WEBSOCKETPP_NO_CPP11_FUNCTIONAL_)
elseif(APPLE)
    # macOS 不需要特殊处理
else()
    # Linux 静态编译：尝试链接静态库
    target_link_libraries(mini-wsbroad PRIVATE -static-libgcc -static-libstdc++ -pthread)
endif()
