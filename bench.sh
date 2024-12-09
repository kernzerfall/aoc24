#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Expects the input for dayXY to be in ./build/inputXY
day_command() {
  printf "${SCRIPT_DIR}/build/d$1/d$1 b ${SCRIPT_DIR}/build/input$1"
}

declare -a bench_targets

for i in $(seq 1 30); do
  printf -v d %02d $i
  if ! [[ -f "${SCRIPT_DIR}/build/d$d/d$d" ]]; then
    continue
  fi
  bench_targets+=("$(day_command $d)")
done

echo ${bench_targets[@]}

hyperfine \
  --export-markdown "${SCRIPT_DIR}/build/bench.md" \
  -N --prepare 'true' --warmup 1024 --min-runs 1024 \
  -- "${bench_targets[@]}"
