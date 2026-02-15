#include "head.h"

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
   广播函数
   ========================= */
void broadcast_json(const json &msg) {
  std::lock_guard<std::mutex> lock(clients_mutex);
  for (auto &hdl : clients) {
    try {
      ws_server->send(hdl, msg.dump(), websocketpp::frame::opcode::text);
    } catch (const std::exception &e) {
      std::cout << "[ERROR] Failed to send to client: " << e.what() << "\n";
    }
  }
}

/* =========================
   客户端消息处理
   ========================= */
void on_message(connection_hdl hdl, server::message_ptr msg) {
  try {
    std::string payload = msg->get_payload();
    json j = json::parse(payload);

    broadcast_json(j);
    std::cout << "[Broadcast] Message from client broadcasted\n";

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

  if (prefix == "server" || prefix == "sv") {
    if (suffix == "on")
      server_on();
    else if (suffix == "off")
      server_off();
    else if (suffix == "reboot" || suffix == "rb")
      server_reboot();
    else
      std::cout << "[!] Unknown server command\n";
    time_sleep();
  } else if (prefix == "broadcast" || prefix == "bt") {
    // broadcast <file_path>
    if (suffix.empty()) {
      std::cout << "[!] Missing file path\n";
      return;
    }

    std::ifstream f(suffix);
    if (!f.is_open()) {
      std::cout << "[!] Failed to open file: " << suffix << "\n";
      return;
    }

    try {
      json j;
      f >> j;
      broadcast_json(j);
      std::cout << "[Broadcast] Broadcasted JSON from file: " << suffix << "\n";
    } catch (const std::exception &e) {
      std::cout << "[ERROR] Failed to parse JSON file: " << e.what() << "\n";
    }
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

  std::cout << "WebSocket Control Console\n";
  std::cout << "Commands:\n";
  std::cout << "  server/sv on/off/reboot\n";
  std::cout << "  broadcast/bt <file_path>\n";
  std::cout << "  exit\n\n";

  std::string line;
  while (true) {
    std::cout << ">> ";
    if (!std::getline(std::cin, line))
      break;
    handle_command(line);
  }

  program_exit();
}
