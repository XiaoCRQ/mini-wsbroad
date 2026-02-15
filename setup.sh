#!/usr/bin/env bash

set -e

echo "[Info] Installing required libraries..."
sudo pacman -S --needed --noconfirm websocketpp asio nlohmann-json

echo "[Info] Compiling mini-wsbroad..."
g++ -O2 main.cpp -o mini-wsbroad

echo "[Info] Installing to ~/.local/bin..."
mkdir -p ~/.local/bin
cp ./mini-wsbroad ~/.local/bin/

if ! echo "$PATH" | grep -q "$HOME/.local/bin"; then
  echo "[Warning] ~/.local/bin is not in your PATH."
  echo "Add this line to your shell config (~/.bashrc or ~/.zshrc):"
  echo 'export PATH="$HOME/.local/bin:$PATH"'
fi

echo "[Info] Installation complete! Run 'mini-wsbroad <host> <port>' anywhere."
