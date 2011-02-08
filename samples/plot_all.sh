# /bin/sh

./graph.sh sop-sog-time.eps -v SOP SOG 7 $1/**/*.log
./graph.sh sog-sop-time.eps -v SOG SOP 7 $1/**/*.log
./graph.sh sog-slap-time.eps -v SOG SLAP 7 $1/**/*.log
./graph.sh sog-fsltl-time.eps -v SOG FSLTL 7 $1/**/*.log
./graph.sh sop-fsltl-time.eps -v SOP FSLTL 7 $1/**/*.log
./graph.sh slap-sop-time.eps -v SLAP SOP 7 $1/**/*.log
./graph.sh slap-fsltl-time.eps -v SLAP FSLTL 7 $1/**/*.log
./graph.sh slap-slapfst-time.eps -v SLAP SLAP-FST 7 $1/**/*.log
./graph.sh slap-slapfsa-time.eps -v SLAP SLAP-FSA 7 $1/**/*.log
./graph.sh slapfst-slapfsa-time.eps -v SLAP-FSA SLAP-FST 7 $1/**/*.log


./graph.sh sog-sop-mem.eps -v SOG SOP 8 $1/**/*.log
./graph.sh sog-slap-mem.eps -v SOG SLAP 8 $1/**/*.log
./graph.sh sog-fsltl-mem.eps -v SOG FSLTL 8 $1/**/*.log
./graph.sh sop-fsltl-mem.eps -v SOP FSLTL 8 $1/**/*.log
./graph.sh slap-sop-mem.eps -v SLAP SOP 8 $1/**/*.log
./graph.sh slap-fsltl-mem.eps -v SLAP FSLTL 8 $1/**/*.log
./graph.sh slap-slapfst-mem.eps -v SLAP SLAP-FST 8 $1/**/*.log
./graph.sh slap-slapfsa-mem.eps -v SLAP SLAP-FSA 8 $1/**/*.log
./graph.sh slapfst-slapfsa-mem.eps -v SLAP-FSA SLAP-FST 8 $1/**/*.log
