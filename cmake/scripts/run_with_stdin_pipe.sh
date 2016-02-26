#!/bin/sh

# Expected Parameters:
# $1 program to run
# $2 file which should be piped into stdin

if [ $# -ne 2 ]; then
  echo "usage: $0 <program> <file for stdin>"
  exit -1
fi

"$1" < "$2"
