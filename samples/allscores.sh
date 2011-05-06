#!/bin/sh



if test $# -ne 1; then
  echo "syntax: allscores.sh logfile"
  exit 1
fi

# set -x

f=tmp.$$
grep -v '""' $1 > $f


grep -v SLAP-FSA $f | grep 'FS[^AT]' | ./winner.pl 7 - > $1.fsalgos.time
grep -v SLAP-FSA $f | grep 'FS[^AT]' | ./winner.pl 8 - > $1.fsalgos.mem

grep -v SLAP-FSA $f | grep 'SO[GP]' | ./winner.pl 7 - > $1.sogsop.time
grep -v SLAP-FSA $f | grep 'SO[GP]' | ./winner.pl 8 - > $1.sogsop.mem


grep -vE '(SLAP-FSA|FSOWCTY|FSEL)' $f | grep -v 'X[^W]' | ./winner.pl 3 - > $1.products

grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 7 - > $1.nox.time
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 8 - > $1.nox.mem
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') |
                                        ./winner.pl 7 - > $1.x.time
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') |
                                        ./winner.pl 8 - > $1.x.mem

for f in $1.fsalgos.time $1.fsalgos.mem $1.sogsop.mem $1.sogsop.time $1.products $1.nox.time $1.nox.mem $1.x.time $1.x.mem; do
  prefix=`basename $f | sed 's/[_.]log//g'`
  ./plotscore.sh $prefix $f
done

rm -f *.time *.mem
#rm -f $1.fsalgos.time $1.fsalgos.mem  $1.products $1.nox.time $1.nox.mem $1.x.time $1.x.mem

