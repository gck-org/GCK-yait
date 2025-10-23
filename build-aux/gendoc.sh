#!/bin/sh

# Usage: $0 [options]...

prog_name=$(basename $0)
tool_version="beta"
year=2025

fatal() {
	echo "fatal: $*" >&2
	exit 1
}

run() {
	"$@" || fatal "could not run: $*"
}

print_help() {
    cat <<EOF
Usage: $prog_name [options]...

      --help     print this help and exit.
      --version  print version information.
EOF
}

print_version() {
    cat <<EOF
$prog_name $tool_version $(git rev-list --count --all 2>/dev/null || echo 0)
Copyright (C) $year vx-clutch.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
EOF
}

header=$(cat <<EOF

EOF)

while [ $# -gt 0 ]; do
    case "$1" in
        --help) print_help; exit 0 ;;
        --version) print_version; exit 0 ;;
	*) fatal "Not implemented yet" ;;
    esac
    shift
done
