#! /bin/sh

echo "##teamcity[testSuiteStarted name='its-ltl.test']"

for i in *.data ; do
    ./run_test.pl $i ;
done;


echo "##teamcity[testSuiteFinished name='its-ltl.test']"

