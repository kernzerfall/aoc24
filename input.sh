#!/bin/sh

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

INPUT_URL="https://adventofcode.com/2024/day/%d/input"
HEADER="Cookie: session=$COOKIE"

get_input() {
  printf -v fname "input%02d" $1
  curl \
    -H "$HEADER" \
    -o "${SCRIPT_DIR}/build/$fname" \
    $(printf $INPUT_URL $1)
}

# if we have an arg, get the input specified by it
if [[ $# -ge 1 ]]; then
  get_input $1
  exit 0
fi

# otherwise, get all missing inputs
# (missing <=> we have an executable for the day but no input)
for i in $(seq 1 30); do
  printf -v d %02d $i
  if ! [[ -f "${SCRIPT_DIR}/build/d$d/d$d" ]]; then
    continue
  fi

  if [[ -f "${SCRIPT_DIR}/build/input$d" ]]; then
    continue
  fi

  echo "Missing input for d$d"
  get_input $i
done


