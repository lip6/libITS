#!/bin/zsh

if test $# -ne 1; then
  echo "syntax: sum_all.sh directory"
  exit 1
fi

if test ! -d "$1"; then
  echo "$1 is not a directory"
  exit 1
fi

for i in SOG SOP BCZ99 \
         SLAP SLAP-FSA SLAP-FST \
         'FSEL -dR3' FSEL 'FSEL -R3f' \
         'FSOWCTY -dR3' FSOWCTY 'FSOWCTY -R3f'
do
  echo "=== $i"
  ./sum.pl -f "^$i," $1/**/*.log
done
