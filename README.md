# 📡 mini-wsbroad — 轻量级 WebSocket 广播服务器

一个用 C++ 编写的极简 WebSocket 广播服务，专为实时消息分发设计。  
只广播带有 `"action": "broadcast"` 的 JSON 消息，干净、高效、无多余逻辑。

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
**仅当收到如下格式的 JSON 时，消息才会被广播给所有客户端：**

```json
{
  "action": "broadcast",
  ...
}
```

>[!WARNING]
>不带 `"action": "broadcast"` 字段的消息将被忽略

---

## 🖥️ 内置控制命令（通过标准输入）

- `server on`    — 启动服务（若尚未运行）
- `server off`   — 停止服务（保持程序运行）
- `server reboot`— 重启服务
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
