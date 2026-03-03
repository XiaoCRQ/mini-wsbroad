# 📡 mini-wsbroad — 轻量级 WebSocket 广播服务器

一个用 C++ 编写的极简 WebSocket 广播服务，专为实时消息分发设计。
基于：

* WebSocket++
* Asio（Standalone 模式）
* nlohmann/json

特点：

* 单进程高性能广播
* 多客户端连接管理
* JSON 消息自动转发
* 支持标准输入控制命令
* 无 TLS 依赖（asio_no_tls）

---

# 🛠️ 编译指南

> 本项目使用 **Standalone Asio 模式**，无需 Boost。

---

## 🐧 Arch Linux（已支持）

```bash
cd ~/.cache
git clone https://github.com/XiaoCRQ/mini-wsbroad 
cd mini-wsbroad
./setup.sh
```

---

# 🪟 Windows 编译教程

## 方式一：使用 MSYS2（推荐）

### 1️⃣ 安装 MSYS2

下载并安装：
[https://www.msys2.org/](https://www.msys2.org/)

打开 **MSYS2 MinGW64 Shell**

---

### 2️⃣ 安装依赖

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-websocketpp \
          mingw-w64-x86_64-asio \
          mingw-w64-x86_64-nlohmann-json
```

---

### 3️⃣ 编译

```bash
g++ -std=c++17 -O2 main.cpp \
    -lws2_32 \
    -pthread \
    -static \
    -o mini-wsbroad.exe
```

---

## 方式二：使用 MinGW（手动头文件）

如果你使用原生 MinGW：

1. 下载源码版本：

   * WebSocket++
   * Asio（Standalone 版本）
   * nlohmann/json 单头文件

2. 放入 `include/` 目录，例如：

```
project/
 ├── main.cpp
 └── include/
     ├── websocketpp/
     ├── asio/
     └── nlohmann/
```

1. 编译：

```bash
g++ -std=c++17 -O2 main.cpp \
    -Iinclude \
    -lws2_32 \
    -pthread \
    -static \
    -o mini-wsbroad.exe
```

---

# 🍎 macOS 编译教程

推荐使用 Homebrew。

## 1️⃣ 安装 Homebrew（如未安装）

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

---

## 2️⃣ 安装依赖

```bash
brew install websocketpp asio nlohmann-json
```

---

## 3️⃣ 编译

```bash
clang++ -std=c++17 -O2 main.cpp \
        -pthread \
        -static \
        -o mini-wsbroad
```

如果 Homebrew 未自动配置头文件路径，可使用：

```bash
clang++ -std=c++17 -O2 main.cpp \
        -I/opt/homebrew/include \
        -L/opt/homebrew/lib \
        -pthread \
        -o mini-wsbroad
```

---

# ▶️ 使用方式

```bash
./mini-wsbroad <host> <port>
```

例如：

```bash
./mini-wsbroad 0.0.0.0 9002
```

启动后：

* 任意 WebSocket 客户端可连接
* 任意客户端发送的 `JSON` 数据都会广播给所有连接者

---

# 🖥️ 内置控制命令（标准输入）

| 命令                               | 说明      |
| -------------------------------- | ------- |
| `broadcast <file>` / `bt <file>` | 广播文件内容  |
| `status`                         | 显示当前连接数 |
| `exit`                           | 关闭服务器   |

---

# 🔄 更新

```bash
git pull
./setup.sh
```

---

# 🗑 卸载

## Arch Linux

删除可执行文件：

```bash
sudo rm /usr/local/bin/mini-wsbroad
```

删除依赖（如果通过 pacman 安装）：

```bash
sudo pacman -Rns websocketpp asio nlohmann-json
```

---

# ⚙️ 编译要求

* C++17 及以上
* 支持 pthread
* Windows 需链接 `ws2_32`
