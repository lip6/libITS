#!/bin/sh

set -e

RANDLTL=/home/dp/spot/src/ltltest/randltl
CHECKSOG=/home/dp/checksog-state/check-sog
CHECKPN=/home/dp/checkpn-0.0b/src/checkpn
TIME='/usr/bin/time -p'
MODEL='philo6.net'
BOUND='1'

$RANDLTL -p 'X=0.0' -F 10000 -u -s 1 -f 5 -r 5 `cat philo.ap` |
while read f; do
  echo formula $f
  $CHECKSOG -aCou99 -c -f"$f"  $MODEL $BOUND | grep accepting > sog.res
  $CHECKPN -f"$f" $MODEL | grep accepting > pn.res
  mavar=`diff sog.res pn.res | wc -l`
  if [ $mavar -ne 0 ]
  then
    echo bug with $f
    exit 0
  fi
  rm sog.res pn.res
done
