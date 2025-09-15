#!/bin/sh

yait="$(pwd)/bin/yait"

fatal() {
    echo "fatal: $*" >&2
    exit 1
}

[ -d "./yait" ] && fatal "must be run from parent directory"

{
$yait --help || fatal "failed on --help"
$yait --version || fatal "failed on --version"

tmpd=$(mktemp -d)
cd $tmpd

$yait foo || fatal "failed to create `foo` at ${tmpd}"
} > build/test.log 2>&1
