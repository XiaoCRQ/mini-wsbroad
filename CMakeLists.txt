cmake_minimum_required(VERSION 3.14)
project(mini-wsbroad VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Linux 静态链接处理 (适配 Arch/Ubuntu 等)
if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

include(FetchContent)

# 依赖管理
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_Declare(asio URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz)
FetchContent_Declare(websocketpp GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git GIT_TAG master)

FetchContent_MakeAvailable(json asio websocketpp)

add_executable(mini-wsbroad main.cpp)

# 核心：解决 Windows/MSVC 强制寻找 Boost 的问题
target_compile_definitions(mini-wsbroad PRIVATE 
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STANDALONE_ASIO_
    _WEBSOCKETPP_CPP11_FUNCTIONAL_
    _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
    _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
    _WEBSOCKETPP_CPP11_MEMORY_
    _WEBSOCKETPP_CPP11_CHRONO_
    _WEBSOCKETPP_CPP11_TYPE_TRAITS_
    _WEBSOCKETPP_CPP11_THREAD_
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
    # 彻底关掉 Windows 上的 Boost 探测
    target_compile_definitions(mini-wsbroad PRIVATE BOOST_ALL_NO_LIB)
elseif(UNIX AND NOT APPLE)
    # 静态链接线程和 C++ 库
    target_link_libraries(mini-wsbroad PRIVATE -pthread)
endif()
