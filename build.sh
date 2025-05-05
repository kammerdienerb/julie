#!/usr/bin/env bash

ASAN="-fsanitize=address"
CFLAGS="-Isrc -Wall -pedantic -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -g -O0 ${ASAN}"
# CFLAGS="-Isrc -O3 -g"
LDFLAGS="-ldl -lm"

echo "CC julie"
gcc -o julie src/driver.c ${CFLAGS} ${LDFLAGS} || exit $?

# for f in packages/*.c; do
#     SO=$(dirname $f)/$(basename $f .c).so
#     echo "CC ${SO}"
#     gcc -shared -fPIC -o ${SO} $f ${CFLAGS} ${LDFLAGS} -lm || exit $?
# done
