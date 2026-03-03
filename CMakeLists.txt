cmake_minimum_required(VERSION 3.14)
project(mini-wsbroad VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 下载 nlohmann/json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# 下载 websocketpp
FetchContent_Declare(websocketpp GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git GIT_TAG master)
FetchContent_MakeAvailable(websocketpp)

# 查找 Asio (通常系统自带或通过包管理器，此处假设环境已准备好，或在 Action 中处理)
# 为简化，Action 脚本中会安装 libasio-dev

add_executable(mini-wsbroad main.cpp)
target_link_libraries(mini-wsbroad PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(mini-wsbroad PRIVATE ${websocketpp_SOURCE_DIR})

# 链接线程库
find_package(Threads REQUIRED)
target_link_libraries(mini-wsbroad PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(mini-wsbroad PRIVATE /W4)
else()
    target_compile_options(mini-wsbroad PRIVATE -Wall -Wextra)
endif()
