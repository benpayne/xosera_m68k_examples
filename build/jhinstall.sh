#!/bin/sh

#echo invocation: $*
#echo num params: $#
let N=$#
echo dest is ${!N}
for ((i=1; i<N; ++i)); do
  if [ -e ${!i} ]; then
    echo cp ${!i} ${!N}
    cp ${!i} ${!N}
  else
    echo ${!i} not found, skipping
  fi
done
