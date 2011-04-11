
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
echo FSEL
./sum.pl -f '^FSEL' $1/**/*.log
echo FSOWCTY
./sum.pl -f '^FSOWCTY' $1/**/*.log
