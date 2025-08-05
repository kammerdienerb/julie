#!/usr/bin/env bash

PCRE2_CFLAGS=""
PCRE2_LDFLAGS=""
if which pcre2-config > /dev/null && ! [[ $(pcre2-config --version) < "10.36" ]]; then
    PCRE2_CFLAGS="$(pcre2-config --cflags-posix) -DJULIE_USE_PCRE2"
    PCRE2_LDFLAGS="$(pcre2-config --libs-posix)"
fi

# ASAN="-fsanitize=address"
# CFLAGS="-Isrc -rdynamic -Wall -pedantic -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -g -O0 ${ASAN} ${PCRE2_CFLAGS}"
CFLAGS="-Isrc -rdynamic -O3 -march=native -mtune=native -g -DJULIE_ASSERTIONS=0 ${PCRE2_CFLAGS}"
LDFLAGS="-ldl -lm ${PCRE2_LDFLAGS}"

echo "CC julie"
gcc -o julie src/driver.c ${CFLAGS} ${LDFLAGS} || exit $?
