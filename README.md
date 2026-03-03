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

# ▶️ 使用方式

```bash
./mini-wsbroad <host> <port>
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
