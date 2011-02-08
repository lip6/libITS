#! /bin/sh

echo "##teamcity[testSuiteStarted name='its-ctl.test']"

for i in *.data ; do
    ./run_test.pl $i ;
done;


echo "##teamcity[testSuiteFinished name='its-ctl.test']"

