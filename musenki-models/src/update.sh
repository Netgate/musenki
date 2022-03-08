#!/bin/sh

if [ ! -d "openconfig" ]; then
    mkdir openconfig
fi

WORKDIR=`mktemp -d`

(
    cd $WORKDIR
    wget https://github.com/openconfig/public/archive/refs/heads/master.zip
    unzip master.zip
)

cp -pr $WORKDIR/public-master/release/models/* ./openconfig

rm -rf $WORKDIR
echo $WORKDIR

echo "Last Fetched: `date`" > openconfig/UPDATED






