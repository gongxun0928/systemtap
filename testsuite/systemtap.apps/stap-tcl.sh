#! /bin/sh

set -e

tclreleasemajor="8.6"
tclrelease="8.6b1"
tcldir=`pwd`/tcl/install/

mkdir -p tcl

if [ ! -r tcl$tclrelease-src.tar.gz ] ; then
    wget http://sourceforge.net/projects/tcl/files/Tcl/$tclrelease/tcl$tclrelease-src.tar.gz/download
fi

if [ ! -d tcl/src ] ; then
    tar -x -z -f tcl$tclrelease-src.tar.gz
    mv tcl$tclrelease tcl/src
fi

cd tcl/src/unix
env CPPFLAGS="-I$SYSTEMTAP_INCLUDES" CFLAGS="-g -O2" ./configure --prefix=$tcldir --enable-dtrace 
make -j2
make install

exit 0