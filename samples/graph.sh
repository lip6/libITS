#!/bin/sh


if [ $# -le 4 ]; then
    echo "syntax: graph.sh output.ps [-v] meth1 meth2 column files..." >&2
    exit 2;
fi

output=$1
shift

echo "Gathering data..."

./graphdata.pl $opt "$@" >$output.data

[ "x$1" = "x-v" ] && shift

models=`cut -f 1 -d ' ' $output.data | sort -u | tr '\n' ' '`


cat > $output.gnuplot  <<EOF
set terminal postscript eps enhanced color
set xlabel "$1"
set ylabel "$2"
set logscale x
set logscale y
#unset key
set key left
set output '$output'

# Use some jitter to distinguish points that would otherwise be equal
# (the jitter is multiplicative because the scale is logarithmic)
spread=10 # maximum percentage added or substracted to the real value
jitter(x) = x*(100+2*spread*(rand(0)-0.5))/100

plot \\
EOF


echo "Plots: $models"
for i in $models; do
  sed -n "s/^$i \(.*\)$/\\1/p" < $output.data > $output.$i.data
  echo "'$output.$i.data' using (jitter(\$1)):(jitter(\$2)) with points pointtype 1  title \"$i\", \\" >> $output.gnuplot
done

echo '  x notitle' >> $output.gnuplot

echo "Rendering graph..."

gnuplot $output.gnuplot
