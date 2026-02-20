#define _WEBSOCKETPP_CPP11_STANDALONE_ASIO
#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>

#include "nlohmann/json.hpp"

using json = nlohmann::json;
using server = websocketpp::server<websocketpp::config::asio>;

// -----------------------------
// 全局状态
// -----------------------------
std::set<websocketpp::connection_hdl,
         std::owner_less<websocketpp::connection_hdl>>
    clients;
std::mutex clients_mutex;
std::atomic<bool> running(true);

// -----------------------------
// 广播 JSON
// -----------------------------
void broadcastJsonFile(server &s, const std::string &filePath) {
  std::ifstream inFile(filePath);
  if (!inFile.is_open()) {
    std::cout << "[ERROR] File not found: " << filePath << std::endl;
    return;
  }

  std::stringstream buffer;
  buffer << inFile.rdbuf();
  std::string content = buffer.str();

  json j;
  try {
    j = json::parse(content);
  } catch (const std::exception &e) {
    std::cout << "[ERROR] Invalid JSON: " << e.what() << std::endl;
    return;
  }

  std::string data = j.dump();
  int count = 0;

  std::lock_guard<std::mutex> lock(clients_mutex);
  for (auto hdl : clients) {
    try {
      s.send(hdl, data, websocketpp::frame::opcode::text);
      count++;
    } catch (...) {
      // ignore send errors
    }
  }

  std::cout << "[WS] Broadcast complete -> " << count << " clients"
            << std::endl;
}

// -----------------------------
// 命令行线程
// -----------------------------
void commandLoop(server &s) {
  std::string line;
  while (running) {
    std::cout << ">> " << std::flush;
    if (!std::getline(std::cin, line))
      break;

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "broadcast" || cmd == "bt") {
      std::string filePath;
      iss >> filePath;
      if (filePath.empty()) {
        std::cout << "Usage: broadcast <json_file_path>" << std::endl;
        continue;
      }
      broadcastJsonFile(s, filePath);
    } else if (cmd == "status") {
      std::lock_guard<std::mutex> lock(clients_mutex);
      std::cout << "[WS] Connected clients: " << clients.size() << std::endl;
    } else if (cmd == "exit") {
      std::cout << "[WS] Shutting down server..." << std::endl;

      // 断开所有客户端
      {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto hdl : clients) {
          try {
            s.close(hdl, websocketpp::close::status::going_away, "Server exit");
          } catch (...) {
          }
        }
      }

      running = false;
      s.stop_listening();
      break;
    }
  }
}

// -----------------------------
// 主函数
// -----------------------------
int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
    return 1;
  }

  std::string host = argv[1];
  int port = std::stoi(argv[2]);

  server ws_server;

  // 屏蔽 websocketpp 日志输出
  ws_server.clear_access_channels(websocketpp::log::alevel::all);
  ws_server.clear_error_channels(websocketpp::log::elevel::all);

  ws_server.set_reuse_addr(true);
  ws_server.init_asio();

  ws_server.set_open_handler([&](websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.insert(hdl);
    // 仅 std::cout 输出，其他屏蔽
    std::cout << "[WS] Client connected" << std::endl;
    std::cout << ">> " << std::flush;
  });

  ws_server.set_close_handler([&](websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(hdl);
    std::cout << "[WS] Client disconnected" << std::endl;
  });

  ws_server.listen(port);
  ws_server.start_accept();

  std::cout << "[WS] Server listening on " << host << ":" << port << std::endl;

  std::thread cmdThread(commandLoop, std::ref(ws_server));

  // 运行事件循环
  ws_server.run();

  cmdThread.join();
  std::cout << "[WS] Server exited" << std::endl;
  return 0;
}
