#!/bin/sh


if [ $# -le 4 ]; then
    echo "syntax: graph.sh output.ps meth1 meth2 column files..." >&2
    exit 2;
fi

output=$1
shift

echo "Gathering data..."

./graphdata.pl "$@" >$output.data

echo "Rendering graph..."

cat > $output.gnuplot  <<EOF
set terminal postscript eps enhanced color
set xlabel "$1"
set ylabel "$2"
set logscale x
set logscale y
unset key
set output '$output'

# Use some jitter to distinguish points that would otherwise be equal
# (the jitter is multiplicative because the scale is logarithmic)
jitter(x) = x*(9.5+rand(0))/10

plot '$output.data' using (jitter(\$1)):(jitter(\$2)) with dots lw 7, \
     x with lines
EOF

gnuplot $output.gnuplot
