#!/bin/sh
set -e
d=$(dirname "$0")
cd -- "$d"
exec ${CC:-cc} -O2 -Wl,--build-id=none,--hash-style=gnu,--strip-all,--gc-sections -o zip2gzip zip2gzip.c