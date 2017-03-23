
# ---------------
# install antlr
# ---------------

# create a dedicated install dir
mkdir usr
mkdir usr/local
mkdir usr/local/lib
mkdir usr/local/include


# install antlr
wget http://www.antlr3.org/download/antlr-3.4.tar.gz
tar -xzf antlr-3.4.tar.gz

cp antlr-3.4/lib/antlr-3.4-complete.jar ./usr/local/lib/
tmp=$(pwd)
cd ./antlr-3.4/runtime/C
autoreconf -vfi
./configure --prefix="$tmp"/usr/local --enable-64bit
make
make install
cd -

# ---------------
# install libits related tools with :

# ./configure   --with-antlrc="$tmp"/usr/local/   --with-antlrjar="$tmp"/usr/local/lib/antlr-3.4-complete.jar
