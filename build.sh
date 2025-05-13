#!/usr/bin/env bash

# ASAN="-fsanitize=address"
CFLAGS="-Isrc -rdynamic -Wall -pedantic -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -g -O0 ${ASAN}"
# CFLAGS="-Isrc -rdynamic -O3 -march=native -mtune=native -g -DJULIE_ASSERTIONS=0"
LDFLAGS="-ldl -lm"

# echo "CC julie"
# clang -o julie src/driver.c ${CFLAGS} ${LDFLAGS} || exit $?
echo "CC term"
clang -o term term.c ${CFLAGS} ${LDFLAGS} || exit $?

# echo "CC term.so"
# if [[ $(uname -s) == "Darwin" ]]; then
#     CFLAGS+=" -undefined dynamic_lookup"
# fi
# clang -shared -fPIC -o term.so term.c ${CFLAGS} ${LDFLAGS} || exit $?

# for f in packages/*.c; do
#     SO=$(dirname $f)/$(basename $f .c).so
#     echo "CC ${SO}"
#     gcc -shared -fPIC -o ${SO} $f ${CFLAGS} ${LDFLAGS} -lm || exit $?
# done
