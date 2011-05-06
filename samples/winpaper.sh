#!/bin/sh

if test $# -ne 1; then
  echo "syntax: winpaper.sh logfile"
  exit 1
fi

f=tmp.$$
grep -v '""' $1 > $f

(echo '\begin{tabular}{cccrlrlrlrlrlrlrl}'
grep -vE SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl -l 7 - |
  sed 's/hline/hline\\multirow{6}{*}{\\begin{sideways}without $\\X$\\end{sideways}}/;s/FS\([OE]\)/\1/g;s/BCZ99/BCZ/g'
grep -vE SLAP-FSA $f | (echo; grep 'X[^W]') | ./winner.pl -l 7 - |
  sed '1d;s/hline/hline\\multirow{6}{*}{\\begin{sideways}with $\\X$\\end{sideways}}/;s/FS\([OE]\)/\1/g;s/BCZ99/BCZ/g'
echo '\end{tabular}') >tablewin-tr.tex
echo tablewin-tr.tex generated

(echo '\begin{tabular}{cccrlrlrlrlrl}'
grep -vE SLAP-FSA $f | grep -v 'X[^W]' | ./winner.pl -a 7 - |
  sed 's/hline/hline\\multirow{6}{*}{\\begin{sideways}w\/o $\\X$\\end{sideways}}/;s/FS\([OE]\)/\1/g;s/BCZ99/BCZ/g'
grep -vE SLAP-FSA $f | (echo; grep 'X[^W]') | ./winner.pl -a 7 - |
  sed '1d;s/hline/hline\\multirow{6}{*}{\\begin{sideways}with $\\X$\\end{sideways}}/;s/FS\([OE]\)/\1/g;s/BCZ99/BCZ/g'
echo '\end{tabular}') > tablewin-atva.tex
echo tablewin-atva.tex generated

