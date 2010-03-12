#! /bin/sh

SCRIPT_DIR=$(cd "$(dirname $0)"; pwd)
source "$SCRIPT_DIR"/install-lib.sh

INSTALL_PATH_SYSTEMC_LLVM=$(pwd)
build_systemc_for_llvm
