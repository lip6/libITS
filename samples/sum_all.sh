
echo SOG
./sum.pl -f '^SOG' test_bench/**/*.log
echo DSOG
./sum.pl -f '^DSOG' test_bench/**/*.log
echo SLAP
./sum.pl -f '^SLAP,' test_bench/**/*.log
echo SLAP-FSA
./sum.pl -f '^SLAP-FSA,' test_bench/**/*.log
echo SLAP-FST
./sum.pl -f '^SLAP-FST' test_bench/**/*.log
echo FSLTL
./sum.pl -f '^FSLTL' test_bench/**/*.log