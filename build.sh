#!/bin/sh
set -e
d=$(dirname "$0")
cd -- "$d"
exec ${CC:-cc} -Ofast -o zip2gzip zip2gzip.c