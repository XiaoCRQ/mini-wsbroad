#!/usr/bin/env bash
set -e

sudo pacman -S --needed --noconfirm websocketpp asio nlohmann-json

rm bin -rf
mkdir bin

SRC="main.cpp"
LINUX_OUT="bin/mini-wsbroad"

start_time=$(date +%s)

spinner() {
  local pid=$1
  local delay=0.1
  local spinstr='⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏'

  while ps -p $pid >/dev/null 2>&1; do
    local temp=${spinstr#?}
    printf "\r[Info] Building %s " "$spinstr"
    spinstr=$temp${spinstr%"$temp"}
    sleep $delay
  done
}

CXXFLAGS="-O2 -static "

echo "[Info] Building Linux binary..."

g++ $CXXFLAGS "$SRC" -o "$LINUX_OUT" &
PID=$!
spinner $PID
wait $PID

printf "\r[Info] ✔ Linux build finished → %s\n" "$LINUX_OUT"
