#!/usr/bin/env bash
set -euo pipefail

WORK_DIR="${1:-.}"

if [[ ! -f "${WORK_DIR}/CMakeLists.txt" ]]; then
    echo "No CMakeLists.txt found in $(realpath "$WORK_DIR")" >&2
    exit 1
fi

if [[ ! -d "${WORK_DIR}/src" ]]; then
    echo "No 'src' directory found in $(realpath "$WORK_DIR")" >&2
    exit 1
fi

CURRENT_DAY="$(sed -nr 's/^\s*set\(CURRENT_DAY\s+([0-9]+)\s*\).*/\1/p' "$WORK_DIR/CMakeLists.txt")"

if [[ -z "$CURRENT_DAY" ]]; then
    echo "Could not find CURRENT_DAY in CMakeLists.txt" >&2
    exit 1
fi

NEXT_DAY=$((CURRENT_DAY + 1))

sed -i -r 's/^\s*(set\(CURRENT_DAY)\s+[0-9]+\s*/\1 '"$NEXT_DAY"'/' "$WORK_DIR/CMakeLists.txt"

for x in $(seq 1 ${NEXT_DAY}); do 
    OUTPUT_FILE="${WORK_DIR}/src/day${x}.cpp"
    if [[ ! -f "${OUTPUT_FILE}" ]]; then
        cat <<-EOF > "${OUTPUT_FILE}"
#include "utils.hpp"
#include "print.hpp"

DPSG_AOC_MAIN(file) {
    std::string line;
    while (std::getline(file, line)) {
    
    }
    dpsg::println("Answer: {}", 0);
}
EOF
    fi
done

cmake -B "${WORK_DIR}/build" -S "${WORK_DIR}"