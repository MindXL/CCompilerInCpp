#!/bin/bash

cd cmake-build-debug || exit
make || exit

separate() {
  echo -e "\033[33m----------------------------------------\033[0m"
}

separate
echo "Enter source code below:"
separate
read -r source
separate

if ! result_s=$(./ccc "${source}"); then
  exit
fi

while getopts "s" opt; do
  case ${opt} in
  s)
    echo "${result_s}"
    separate
    ;;
  *)
    echo "unknown option"
    exit
    ;;
  esac
done

echo "${result_s}" >tmp.s
clang tmp.s ../driver.c
./a.out

rm tmp.s a.out
