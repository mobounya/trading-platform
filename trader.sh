#!/usr/bin/env bash

function get_top_dir() {
    git rev-parse --show-toplevel
}

TRADER_SOURCE_DIR=$(get_top_dir)
BUILD_DIR="${TRADER_SOURCE_DIR}/build"

function build() {
  cmake -S "$TRADER_SOURCE_DIR" -B "$BUILD_DIR"
  cmake --build "$BUILD_DIR"
}

function run() {
  if [ -r "$TRADER_SOURCE_DIR/.env" ] ; then
    cp "$TRADER_SOURCE_DIR/.env" "$BUILD_DIR/.env"
  else
    echo "Please make sure an .env file exists"
    exit 1
  fi
  command "${BUILD_DIR}/trader" "${@}"
}

CMD=$1

if [[ "$CMD" =~ ^(build|run)$ ]]; then
  case "$CMD" in
    build)
      build
      ;;
    run)
      # FIXME: should run build first to make sure there's a binary to run
      run "${@:2}"
      ;;
    esac
fi
