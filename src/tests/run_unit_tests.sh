#!/bin/sh

fatal() {
    echo "fatal: $*" >&2
    exit 1
}

[ -d "./tests" ] || fatal "must be run from parent directory"

./bin/yait --help 2>&1 || fatal "failed on --help"
./bin/yait --version 2>&1 || fatal "failed on --version"
