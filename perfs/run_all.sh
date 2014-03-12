#! /bin/sh

# Do not name test suite to keep history from older versions active
# echo "##teamcity[testSuiteStarted name='its-perfs.test']"

if ! [ -f ../bin/its-reach ]
then
	echo "Build of ITS-REACH seems to have failed, skipping test suite"
else
for i in *.data ; do
    ./run_test.pl $i ;
done;
fi

# anonymous test suite
# echo "##teamcity[testSuiteFinished name='its-perfs.test']"

