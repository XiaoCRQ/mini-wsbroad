#!/usr/bin/env bash

set -e

echo "[Info] Installing required libraries..."
sudo pacman -S --needed --noconfirm websocketpp asio nlohmann-json

echo "[Info] Compiling mini-wsbroad..."
# g++ -O2 main.cpp -o mini-wsbroad
./compiling.sh

echo "[Info] Installing to /usr/local/bin..."
sudo cp ./mini-wsbroad /usr/local/bin/

echo "[Info] Installation complete! Run 'mini-wsbroad <host> <port>' anywhere."
