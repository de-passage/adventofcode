#!/usr/bin/env bash

err() {
  if [[ -t 2 ]]; then
    echo -e "\e[31m$*\e[0m" >&2
  else
    echo "$*" >&2
  fi
  exit 1
}

CONFIG_FILE="${XDG_CONFIG_HOME:-${HOME}/.config}/aoc/session"

if [[ ! -f "${CONFIG_FILE}" ]]; then
  err "Please create a config file '${CONFIG_FILE}' with the AOC session cookie"
fi

if [[ $# -le 2 ]]; then
  err "Usage: $0 <year> <day> [<output_file>]"
fi

YEAR=$1
DAY=$2
SESSION=$(cat "${CONFIG_FILE}")

OUTPUT_FILE="${3:-input/day${DAY}.txt}"

if ! curl -X GET https://adventofcode.com/${YEAR}/day/${DAY}/input \
  -H "Cookie: session=${SESSION}" \
  -o ${OUTPUT_FILE} ; then
  err "Failed to download input, see ${OUTPUT_FILE} for details"
fi
