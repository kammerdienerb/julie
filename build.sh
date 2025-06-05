#!/usr/bin/env bash

# ASAN="-fsanitize=address"
# CFLAGS="-Isrc -rdynamic -Wall -pedantic -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -g -O0 ${ASAN}"
CFLAGS="-Isrc -rdynamic -O3 -march=native -mtune=native -g -DJULIE_ASSERTIONS=0"
LDFLAGS="-ldl -lm"

echo "CC julie"
gcc -o julie src/driver.c ${CFLAGS} ${LDFLAGS} || exit $?
