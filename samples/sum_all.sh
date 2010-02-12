
echo SOG
./sum.pl -f '^SOG' test_bench/**/*.log
echo DSOG
./sum.pl -f '^DSOG' test_bench/**/*.log
echo SLOG
./sum.pl -f '^SLOG' test_bench/**/*.log
echo FSLTL
./sum.pl -f '^FSLTL' test_bench/**/*.log