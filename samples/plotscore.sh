
if [ $# -ne 2 ]; then
    echo "syntax: plotscore.sh output-prefix scorefile" >&2
    exit 2;
fi

output=$1
scorefile=$2

IFS=,
rm -f $$.score.*
while read meth verdict val; do
  echo $val >> "$$.score.$verdict.$meth"
done < $2

gengraph()
{
  cat > $1.gnu <<EOF
bin(x, s) = s*int(x/s)

set zeroaxis
unset title
set terminal postscript landscape enhanced color solid
set output '$1.ps'
set key left
bw = 0.5
#set boxwidth bw * 0.6
set xrange [-0.1:121]
set yrange [0:*]
#set style fill solid 0.4
EOF
  echo -n 'plot ' >> $1.gnu

  sep=
  for i in $$.score.$2.*; do
    echo -n "$sep'$i' u (bin(\$1,bw)+bw/2):(1.0) s cumul t '${i#$$.score.$2.}'" >> $1.gnu
    sep=', '
  done
  gnuplot $1.gnu
}

gengraph $output-empty 0
gengraph $output-counter 1

rm -f $$.score.*
