#!/bin/sh

if test $# -ne 1; then
  echo "syntax: winall.sh logfile"
  exit 1
fi

f=$1

echo -----------------------------------------
echo Comparaisons sur les tailles des produits
echo -----------------------------------------
echo
grep -vE '(SLAP-FSA|FSOWCTY|FSEL)' $f | grep -v 'X[^W]' | ./winner.pl -m 3 -
echo
echo ------------------------------------------------
echo Comparaisons sur les temps [sans les formules X]
echo ------------------------------------------------
echo
grep -vE SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl -m 7 -
echo
echo -------------------------------------------------
echo Comparaisons sur la memoire [sans les formules X]
echo -------------------------------------------------
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl -m 8 -
echo
echo --------------------------------------------------
echo Comparaisons sur les temps [uniquement formules X]
echo --------------------------------------------------
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl -m 7 -
echo
echo ---------------------------------------------------
echo Comparaisons sur la memoire [uniquement formules X]
echo ---------------------------------------------------
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl -m 8 -
