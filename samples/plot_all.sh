# /bin/sh

./graph.sh sog-dsog-time.eps SOG DSOG 7 test_bench/**/*.log
./graph.sh sog-slog-time.eps SOG SLOG 7 test_bench/**/*.log
./graph.sh sog-fsltl-time.eps SOG FSLTL 7 test_bench/**/*.log
./graph.sh dsog-fsltl-time.eps DSOG FSLTL 7 test_bench/**/*.log
./graph.sh slog-dsog-time.eps SLOG DSOG 7 test_bench/**/*.log
./graph.sh slog-fsltl-time.eps SLOG FSLTL 7 test_bench/**/*.log


./graph.sh sog-dsog-mem.eps SOG DSOG 8 test_bench/**/*.log
./graph.sh sog-slog-mem.eps SOG SLOG 8 test_bench/**/*.log
./graph.sh sog-fsltl-mem.eps SOG FSLTL 8 test_bench/**/*.log
./graph.sh dsog-fsltl-mem.eps DSOG FSLTL 8 test_bench/**/*.log
./graph.sh slog-dsog-mem.eps SLOG DSOG 8 test_bench/**/*.log
./graph.sh slog-fsltl-mem.eps SLOG FSLTL 8 test_bench/**/*.log
