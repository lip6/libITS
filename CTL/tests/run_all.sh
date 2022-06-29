#! /bin/sh

echo "##teamcity[testSuiteStarted name='its-ctl.test']"

if ! [ -f ../bin/its-ctl ]
then
	echo "Build of ITS-CTL seems to have failed, skipping test suite"
else
for i in *.data ; do
    ./run_test.pl $i ;
done;
fi

echo "##teamcity[testSuiteFinished name='its-ctl.test']"

