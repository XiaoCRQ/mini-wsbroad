#!/usr/bin/env bash
echo "[Info] Install the required libraries..."
sudo pacman -S websocketpp asio nlohmann-json
echo "[Info] Compiling mini-wsbroad..."
g++ -O2 main.cpp -o mini-wsbroad
# windows
# g++ main.cpp -std=c++17 -lws2_32 -lpthread
echo "[Info] Installation complete"
