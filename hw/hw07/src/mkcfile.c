#!/bin/sh

if :
then
  echo "#include <stdint.h>"
  echo "const uint8_t midifile[] = {"
  od -Anone -v -t x1 ${1} | sed -e 's/^ /0x/g' -e 's/ /,0x/g' -e 's/$/,/'
  echo "};"
fi > ${1%.mid}.c
