# /bin/sh

./graph.sh sog-dsog-time.eps SOG DSOG 7 test_bench/**/*.log
./graph.sh sog-slap-time.eps SOG SLAP 7 test_bench/**/*.log
./graph.sh sog-fsltl-time.eps SOG FSLTL 7 test_bench/**/*.log
./graph.sh dsog-fsltl-time.eps DSOG FSLTL 7 test_bench/**/*.log
./graph.sh slap-dsog-time.eps SLAP DSOG 7 test_bench/**/*.log
./graph.sh slap-fsltl-time.eps SLAP FSLTL 7 test_bench/**/*.log
./graph.sh slap-slapfst-time.eps SLAP SLAP-FST 7 test_bench/**/*.log


./graph.sh sog-dsog-mem.eps SOG DSOG 8 test_bench/**/*.log
./graph.sh sog-slap-mem.eps SOG SLAP 8 test_bench/**/*.log
./graph.sh sog-fsltl-mem.eps SOG FSLTL 8 test_bench/**/*.log
./graph.sh dsog-fsltl-mem.eps DSOG FSLTL 8 test_bench/**/*.log
./graph.sh slap-dsog-mem.eps SLAP DSOG 8 test_bench/**/*.log
./graph.sh slap-fsltl-mem.eps SLAP FSLTL 8 test_bench/**/*.log
./graph.sh slap-slapfst-mem.eps SLAP SLAP-FST 8 test_bench/**/*.log
