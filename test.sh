#!/bin/bash

cd cmake-build-debug || exit
make

./ccc "$1" > tmp.s
clang tmp.s ../driver.c
echo -e "\033[33m----------------------------------------\033[39m"
./a.out

rm tmp.s a.out
