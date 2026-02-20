#!/usr/bin/env bash
set -e

sudo pacman -S --needed --noconfirm websocketpp asio nlohmann-json

rm bin -rf
mkdir bin

SRC="main.cpp"
LINUX_OUT="bin/mini-wsbroad"
WIN_OUT="bin/mini-wsbroad.exe"

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

# echo "[Info] Building Windows binary..."
#
# if command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1; then
#   x86_64-w64-mingw32-g++ $CXXFLAGS \
#     -static -lws2_32 -lmswsock \
#     "$SRC" -o "$WIN_OUT" &
#   PID=$!
#   spinner $PID
#   wait $PID
#
#   printf "\r[Info] ✔ Windows build finished → %s\n" "$WIN_OUT"
# else
#   echo "[Warn] mingw not found. Skipping Windows build."
# fi
#
# end_time=$(date +%s)
# elapsed=$((end_time - start_time))
#
# echo "[Info] All done in ${elapsed}s"
