#!/bin/sh

if test $# -ne 1; then
  echo "syntax: winall.sh logfile"
  exit 1
fi

f=tmp.$$
grep -v '""' $1 > $f

pr='pr -Tme -w80'

echo -----------------------------------------
echo Comparaisons sur les tailles des produits
echo -----------------------------------------
echo
(echo Sans contre-exemples
echo
grep -vE '(SLAP-FSA|FSOWCTY|FSEL)' $f | ./winner.pl 0 3 -) > file1
(echo Avec contre-examples
echo
grep -vE '(SLAP-FSA|FSOWCTY|FSEL)' $f | ./winner.pl 1 3 -) > file2
$pr file1 file2
echo
echo ------------------------------------------------
echo Comparaisons sur les temps [sans les formules X]
echo ------------------------------------------------
echo
(echo Sans contre-exemples
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 0 7 -) > file1
(echo Avec contre-exemples
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 1 7 -) > file2
(echo Seulement les echecs
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 2 7 -) > file3
$pr file1 file2 file3
echo
echo --------------------------------------------------
echo Comparaisons sur la memoire [sans les formules -X]
echo --------------------------------------------------
echo
(echo Sans contre-exemples
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 0 8 -) > file1
(echo Avec contre-exemples
echo
grep -v SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl 1 8 -) > file2
$pr file1 file2
echo
echo --------------------------------------------------
echo Comparaisons sur les temps [uniquement formules X]
echo --------------------------------------------------
echo
(echo Sans contre-exemples
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl 0 7 -)> file1
(echo Avec contre-exemples
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl 1 7 -)> file2
(echo Seulement les echecs
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl 2 7 -)> file3
$pr file1 file2 file3
echo
echo ---------------------------------------------------
echo Comparaisons sur la memoire [uniquement formules X]
echo ---------------------------------------------------
echo
(echo Sans contre-exemples
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl 0 8 -)> file1
(echo Avec contre-exemples
echo
(echo Header; grep -v SLAP-FSA $f | grep 'X[^W]') | ./winner.pl 1 8 -)> file2
$pr file1 file2

rm -f file1 file2 file3 $f
