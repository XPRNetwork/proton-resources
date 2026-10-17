#!/usr/bin/env bash
# Builds the resources and faucet contracts inside ubuntu:18.04 (see .github/workflows/ci.yml).
#
# eosio-ld from eosio.cdt 1.7 has been hanging on GitHub-hosted runners. If a build stalls, this
# dumps the state of the linker processes and retries the link with variants, so the log shows
# which tool hangs and whether any variant avoids it.
set -euo pipefail

STALL_SECONDS=${STALL_SECONDS:-300}

apt-get update -qq
DEBIAN_FRONTEND=noninteractive apt-get install -y -qq --no-install-recommends \
  /tmp/eosio.cdt.deb cmake make g++ procps strace >/dev/null
eosio-cpp --version
echo "cpus: $(nproc)"
free -m

mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=/usr/opt/eosio.cdt/1.7.0 .. >/dev/null

dump_state() {
  echo "::group::Process state"
  ps -eo pid,ppid,stat,wchan:32,etime,pcpu,rss,args
  for pid in $(pgrep -f 'eosio-ld|wasm-ld|eosio-pp|eosio-cpp|clang' || true); do
    echo "--- pid $pid: $(tr '\0' ' ' < "/proc/$pid/cmdline" 2>/dev/null)"
    grep -E '^(State|Threads|VmRSS):' "/proc/$pid/status" 2>/dev/null || true
    echo "wchan: $(cat "/proc/$pid/wchan" 2>/dev/null)"
    ls -l "/proc/$pid/fd" 2>/dev/null || true
    timeout 5 strace -f -tt -p "$pid" 2>&1 | head -40 || true
  done
  echo "::endgroup::"
}

try_link() {
  local name=$1
  shift
  echo "::group::Link variant: $name"
  echo "+ $*"
  local rc=0
  timeout 90 "$@" < /dev/null || rc=$?
  if [ "$rc" -eq 0 ]; then
    echo "RESULT $name: OK"
  elif [ "$rc" -eq 124 ]; then
    echo "RESULT $name: HUNG (killed after 90s)"
  else
    echo "RESULT $name: FAILED (exit $rc)"
  fi
  echo "::endgroup::"
}

link_variants() {
  local dir=$1
  local objs
  objs=$(ls "$dir"/CMakeFiles/atom.dir/src/*.obj 2>/dev/null || true)
  if [ -z "$objs" ]; then
    echo "No object files in $dir; the stall happened before linking."
    return
  fi
  try_link "default" eosio-ld $objs -o /tmp/default.wasm
  try_link "no-post-pass" eosio-ld -fno-post-pass $objs -o /tmp/no-post-pass.wasm
  try_link "lto-O0" eosio-ld -lto-opt=O0 $objs -o /tmp/lto-o0.wasm
  try_link "single-thread" taskset -c 0 eosio-ld $objs -o /tmp/single-thread.wasm
  sha256sum /tmp/*.wasm 2>/dev/null || true
}

build_target() {
  local target=$1
  cmake --build . --target "$target" -- VERBOSE=1 < /dev/null &
  local pid=$!
  local waited=0
  while kill -0 "$pid" 2>/dev/null; do
    if [ "$waited" -ge "$STALL_SECONDS" ]; then
      echo "::error::Build of $target stalled for ${STALL_SECONDS}s"
      dump_state
      pkill -f 'eosio-ld|wasm-ld|eosio-pp' || true
      kill "$pid" 2>/dev/null || true
      wait "$pid" 2>/dev/null || true
      link_variants "$target"
      return 1
    fi
    sleep 5
    waited=$((waited + 5))
  done
  wait "$pid"
}

build_target resources
build_target faucet
