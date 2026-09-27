#!/usr/bin/env bash
# ==========================================================
#  CSE 3206 Lab 3 - Group 7 : one-command build (Linux / macOS / Git Bash)
#  Usage:  ./build.sh        build + run unit tests
#          ./build.sh run    ... then run both demos
# ==========================================================
set -euo pipefail
cd "$(dirname "$0")"
FLAGS="-std=c++17 -Wall -Wextra -Wpedantic"
mkdir -p bin

ITER_SRC="iterator/src/CampusConnect.cpp iterator/src/FeedIterators.cpp iterator/src/FeedScreen.cpp"
MED_SRC="mediator/src/User.cpp mediator/src/Member.cpp mediator/src/SupportBot.cpp mediator/src/MessengerServer.cpp"

if [[ -f iterator/main.cpp ]]; then
    echo "[1/4] Building iterator_demo ..."
    g++ $FLAGS -Iiterator/include $ITER_SRC iterator/main.cpp -o bin/iterator_demo
fi
if [[ -f mediator/main.cpp ]]; then
    echo "[2/4] Building mediator_demo ..."
    g++ $FLAGS -Imediator/include $MED_SRC mediator/main.cpp -o bin/mediator_demo
fi
if [[ -f tests/test_iterator.cpp && -f iterator/main.cpp ]]; then
    echo "[3/4] Building + running test_iterator ..."
    g++ $FLAGS -Iiterator/include -Itests $ITER_SRC tests/test_iterator.cpp -o bin/test_iterator
    ./bin/test_iterator
fi
if [[ -f tests/test_mediator.cpp && -f mediator/main.cpp ]]; then
    echo "[4/4] Building + running test_mediator ..."
    g++ $FLAGS -Imediator/include -Itests $MED_SRC tests/test_mediator.cpp -o bin/test_mediator
    ./bin/test_mediator
fi

echo
echo "Build OK. Executables are in ./bin"
if [[ "${1:-}" == "run" ]]; then
    [[ -x bin/iterator_demo ]] && ./bin/iterator_demo
    [[ -x bin/mediator_demo ]] && ./bin/mediator_demo
fi
