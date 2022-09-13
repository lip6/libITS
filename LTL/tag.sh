#! /bin/sh

DD=`date +'%Y%m%d%H%M%S'`
sed -i  configure.ac -e "s/0\.2/0.2.$DD/"
cd website
sed -i  index.html -e "s/DATETIME/$DD/"
cd ..
