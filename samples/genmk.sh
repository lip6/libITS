#!/bin/sh

if [ $# != 1 ]; then
   echo "Missing directory." >&2
   exit 1
fi

(echo FILES = ; find $1 -name '*.net' -print) | sed '2,$s/^/  /;$q;s/$/ \\/'  > $1.mk

cat <<'EOF' >> $1.mk

all: $(FILES:.net=.log)

.SUFFIXES: .net .log
.net.log:
	./bench2.pl -m all -f $< > $@.tmp
	mv $@.tmp $@
EOF

echo "Now run 'make -f $1.mk'."
