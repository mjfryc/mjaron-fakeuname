#!/bin/bash
set -e;

THIS_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd );
PROJECT_DIR="${THIS_SCRIPT_DIR}";
BUILD_DIR="${PROJECT_DIR}/build";
mkdir -p "${BUILD_DIR}";
pushd "${BUILD_DIR}";
cat "${PROJECT_DIR}/mjaron-fakeuname.h" "${PROJECT_DIR}/mjaron-fakeuname.c" > "${BUILD_DIR}/mjaron-fakeuname-all.c";
cmake "${PROJECT_DIR}";
cmake --build .
popd;

