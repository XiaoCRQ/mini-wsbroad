#!/usr/bin/env bash
set -e

OUTPUT="mini-wsbroad"
SRC="main.cpp"

start_time=$(date +%s)

spinner() {
  local pid=$1
  local delay=0.1
  local spinstr='⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏'

  while ps -p $pid >/dev/null 2>&1; do
    local temp=${spinstr#?}
    printf "\r[Info] Compiling %s %s " "$OUTPUT" "$spinstr"
    spinstr=$temp${spinstr%"$temp"}
    sleep $delay
  done
}

g++ -O2 "$SRC" -o "$OUTPUT" &
PID=$!

spinner $PID
wait $PID

end_time=$(date +%s)
elapsed=$((end_time - start_time))

printf "\r[Info] ✔ Build finished: %s (%ss)\n" "$OUTPUT" "$elapsed"
