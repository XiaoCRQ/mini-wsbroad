#include <atomic>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using json = nlohmann::json;

/* =========================
   全局服务器对象 & 状态
   ========================= */
std::unique_ptr<server> ws_server;
std::atomic<bool> server_running{false};
std::thread server_thread;
std::string g_host;
uint16_t g_port;

inline void time_sleep(long time = 50) {
  std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

/* =========================
   WebSocket 回调
   ========================= */
std::set<connection_hdl, std::owner_less<connection_hdl>> clients;
std::mutex clients_mutex;

void on_open(connection_hdl hdl) {
  std::lock_guard<std::mutex> lock(clients_mutex);
  clients.insert(hdl);
  std::cout << "[+] Client connected\n";
}

void on_close(connection_hdl hdl) {
  std::lock_guard<std::mutex> lock(clients_mutex);
  clients.erase(hdl);
  std::cout << "[-] Client disconnected\n";
}

/* =========================
   客户端消息处理 API
   ========================= */
void broadcast_json(const json &msg) {
  std::lock_guard<std::mutex> lock(clients_mutex);

  json to_send = msg;
  to_send.erase("action");

  for (auto &hdl : clients) {
    try {
      ws_server->send(hdl, to_send.dump(), websocketpp::frame::opcode::text);
    } catch (const std::exception &e) {
      std::cout << "[ERROR] Failed to send to client: " << e.what() << "\n";
    }
  }
}

void handle_client_api(const json &msg, connection_hdl hdl) {
  try {
    if (!msg.contains("action")) {
      ws_server->send(hdl, "Invalid JSON: missing 'action'",
                      websocketpp::frame::opcode::text);
      return;
    }

    std::string action = msg["action"];

    if (action == "echo" && msg.contains("data")) {
      std::string data = msg["data"];
      std::cout << "[API] Echo: " << data << "\n";
      ws_server->send(hdl, data, websocketpp::frame::opcode::text);
    } else if (action == "add" && msg.contains("a") && msg.contains("b")) {
      int a = msg["a"];
      int b = msg["b"];
      int sum = a + b;
      std::cout << "[API] Add: " << a << " + " << b << " = " << sum << "\n";
      json resp = {{"result", sum}};
      ws_server->send(hdl, resp.dump(), websocketpp::frame::opcode::text);
    } else if (action == "broadcast") {
      broadcast_json(msg);
      std::cout << "[API] Broadcast message\n";
    } else {
      ws_server->send(hdl, "Unknown action or missing fields",
                      websocketpp::frame::opcode::text);
    }
  } catch (const std::exception &e) {
    ws_server->send(hdl, std::string("Error: ") + e.what(),
                    websocketpp::frame::opcode::text);
  }
}

void on_message(connection_hdl hdl, server::message_ptr msg) {
  try {
    std::string payload = msg->get_payload();
    json j = json::parse(payload);
    handle_client_api(j, hdl);
  } catch (const std::exception &e) {
    std::cout << "[ERROR] Failed to parse client message: " << e.what() << "\n";
    ws_server->send(hdl, "Invalid JSON", websocketpp::frame::opcode::text);
  }
}

/* =========================
   服务器控制函数
   ========================= */
void server_on() {
  if (server_running.load()) {
    std::cout << "[!] Server already running\n";
    return;
  }

  try {
    ws_server = std::make_unique<server>();
    ws_server->init_asio();
    ws_server->set_reuse_addr(true);
    ws_server->set_open_handler(&on_open);
    ws_server->set_close_handler(&on_close);
    ws_server->set_message_handler(&on_message);

    // 使用跨平台的 endpoint
    asio::ip::tcp::endpoint ep(asio::ip::make_address(g_host), g_port);
    ws_server->listen(ep);
    ws_server->start_accept();

    server_thread = std::thread([]() {
      std::cout << "[+] Server started at ws://" << g_host << ":" << g_port
                << "\n";
      ws_server->run();
    });

    server_running.store(true);
  } catch (const std::exception &e) {
    std::cout << "[ERROR] Failed to start server: " << e.what() << "\n";
  }
}

void server_off() {
  if (!server_running.load()) {
    std::cout << "[!] Server not running\n";
    return;
  }

  try {
    ws_server->stop_listening();
    ws_server->stop();

    if (server_thread.joinable())
      server_thread.join();

    ws_server.reset();
    server_running.store(false);

    std::cout << "[-] Server stopped\n";
  } catch (const std::exception &e) {
    std::cout << "[ERROR] Failed to stop server: " << e.what() << "\n";
  }
}

void server_reboot() {
  std::cout << "[*] Rebooting server...\n";
  if (server_running.load())
    server_off();
  time_sleep();
  server_on();
}

void program_exit() {
  std::cout << "[*] Exiting...\n";
  if (server_running.load())
    server_off();
  exit(0);
}

/* =========================
   指令处理模块
   ========================= */
void handle_command(const std::string &line) {
  std::istringstream iss(line);
  std::string prefix, suffix;
  iss >> prefix >> suffix;

  if (prefix == "server") {
    if (suffix == "on")
      server_on();
    else if (suffix == "off")
      server_off();
    else if (suffix == "reboot")
      server_reboot();
    else
      std::cout << "[!] Unknown server command\n";
    time_sleep();
  } else if (prefix == "exit") {
    program_exit();
  }
}

/* =========================
   主函数
   ========================= */
int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <host> <port>\n";
    return 1;
  }

  g_host = argv[1];
  g_port = static_cast<uint16_t>(std::stoi(argv[2]));

  std::cout << "WebSocket Control Console\nCommands:\n"
            << "  server on\n  server off\n  server reboot\n  exit\n\n";

  std::string line;
  while (true) {
    std::cout << ">> ";
    if (!std::getline(std::cin, line))
      break;
    handle_command(line);
  }

  program_exit();
}
