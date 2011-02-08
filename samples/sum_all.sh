
echo SOG
./sum.pl -f '^SOG' $1/**/*.log
echo SOP
./sum.pl -f '^SOP' $1/**/*.log
echo SLAP
./sum.pl -f '^SLAP,' $1/**/*.log
echo SLAP-FSA
./sum.pl -f '^SLAP-FSA,' $1/**/*.log
echo SLAP-FST
./sum.pl -f '^SLAP-FST' $1/**/*.log
echo FSLTL
./sum.pl -f '^FSLTL' $1/**/*.log
