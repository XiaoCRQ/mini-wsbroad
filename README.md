# 📡 mini-wsbroad — 轻量级 WebSocket 广播服务器

一个用 C++ 编写的极简 WebSocket 广播服务，专为实时消息分发设计，干净、高效、无多余逻辑。  

---

## 🛠️ 本地编译
>
> 脚本会自动处理依赖并编译可执行文件到当前目录。

### ArchLinux

```bash
cd ~/.cache
git clone https://github.com/XiaoCRQ/mini-wsbroad 
cd mini-wsbroad
./setup.sh
```

### Windows

```bash
g++ -O2 main.cpp -lws2_32 -lpthread -o mini-wsbroad
```

---

## ▶️ 使用方式

```bash
./mini-wsbroad <host> <port>
```

启动后，任意 WebSocket 客户端均可连接。  
任意客户端发送的 `Json` 数据包都会被广播。

---

## 🖥️ 内置控制命令（通过标准输入）

- `broadcast/bt <file_path>` — 广播文件
- `server/sv on`    — 启动服务
- `server/sv off`   — 停止服务
- `server/sv reboot`— 重启服务
- `exit`         — 停止服务并退出程序

## 卸载

### ArchLinux

- 删除软件

```bash
rm /usr/local/bin/mini-wsbroad
```

- 删除依赖库（如果使用编译安装）

```bash
sudo pacman -Rns websocketpp asio nlohmann-json
```
