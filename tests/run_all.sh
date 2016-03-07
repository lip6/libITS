#! /bin/sh

echo "##teamcity[testSuiteStarted name='its-ltl.test']"

if ! [ -f ../src/its-ctl ]
then
    echo "Build of ITS-LTL seems to have failed, skipping test suite"
else
for i in *.data ; do
    ./run_test.pl $i ;
done;

echo "##teamcity[testSuiteFinished name='its-ltl.test']"

