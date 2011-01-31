# /bin/sh

./graph.sh dsog-sog-time.eps -v DSOG SOG 7 $1/**/*.log
./graph.sh sog-dsog-time.eps -v SOG DSOG 7 $1/**/*.log
./graph.sh sog-slap-time.eps -v SOG SLAP 7 $1/**/*.log
./graph.sh sog-fsltl-time.eps -v SOG FSLTL 7 $1/**/*.log
./graph.sh dsog-fsltl-time.eps -v DSOG FSLTL 7 $1/**/*.log
./graph.sh slap-dsog-time.eps -v SLAP DSOG 7 $1/**/*.log
./graph.sh slap-fsltl-time.eps -v SLAP FSLTL 7 $1/**/*.log
./graph.sh slap-slapfst-time.eps -v SLAP SLAP-FST 7 $1/**/*.log
./graph.sh slap-slapfsa-time.eps -v SLAP SLAP-FSA 7 $1/**/*.log
./graph.sh slapfst-slapfsa-time.eps -v SLAP-FSA SLAP-FST 7 $1/**/*.log


./graph.sh sog-dsog-mem.eps -v SOG DSOG 8 $1/**/*.log
./graph.sh sog-slap-mem.eps -v SOG SLAP 8 $1/**/*.log
./graph.sh sog-fsltl-mem.eps -v SOG FSLTL 8 $1/**/*.log
./graph.sh dsog-fsltl-mem.eps -v DSOG FSLTL 8 $1/**/*.log
./graph.sh slap-dsog-mem.eps -v SLAP DSOG 8 $1/**/*.log
./graph.sh slap-fsltl-mem.eps -v SLAP FSLTL 8 $1/**/*.log
./graph.sh slap-slapfst-mem.eps -v SLAP SLAP-FST 8 $1/**/*.log
./graph.sh slap-slapfsa-mem.eps -v SLAP SLAP-FSA 8 $1/**/*.log
./graph.sh slapfst-slapfsa-mem.eps -v SLAP-FSA SLAP-FST 8 $1/**/*.log
