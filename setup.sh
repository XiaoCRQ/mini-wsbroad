#!/usr/bin/env bash

set -e

echo "[Info] Installing required libraries..."
sudo pacman -S --needed --noconfirm websocketpp asio nlohmann-json

./compiling.sh

echo "[Info] Installing to /usr/local/bin..."
sudo rm /usr/local/bin/mini-wsbroad
sudo cp ./mini-wsbroad /usr/local/bin/

echo "[Info] Installation complete! Run 'mini-wsbroad <host> <port>' anywhere."
