#!/usr/bin/env bash

# Openwrt build script for musenki wifi app
set -e

STARTTIME=`date`

# Set openwrt CONFIG_TARGET parameters according to make menuconfig
# Default is x86-64
: ${TARGET:=x86}
: ${SUBTARGET:=64}
: ${PROFILE:=generic}

# Where build is made
: ${builddir:=$(pwd)}
test -d ${builddir} || exit -1

# This is the local (created) feed dir (where the wifi application is defined)
: ${localfeeddir=/tmp/musenkilocalfeed}

# Where this script and dependent file reside
: ${srcdir:=$(pwd)}
test -d ${srcdir} || exit -1

# openwrt directory 
: ${openwrtdir:=${builddir}/openwrt}
test -d ${openwrtdir} || exit -1

# Git openwrt verified tag, set TAG=HEAD for latest
: ${TAG:=3d3d03479d5b4a976cf1320d29f4bd4937d5a4ba}

function usage()
{
    echo "usage: $0"
    exit 255
}

if [ $# -ne 0 ]; then 
    usage
fi

cd ${openwrtdir}

git fetch

if [ -z "${NCPUS}" ]; then
    if [ -x "/usr/bin/nproc" ]; then
	NCPUS=`/usr/bin/nproc`
    else
	NCPUS=1
    fi
fi
echo "Builiding with $NCPUS cores"

if [ -n "${TAG}" ]; then 
    git checkout ${TAG}
fi

echo "Make distclean"
echo "=============="
make -j${NCPUS} distclean

# Download cligen/clixon git feed and copy them to a locally created feed
test ! -d ${localfeeddir} || rm -rf ${localfeeddir}
test -d ${localfeeddir} || mkdir ${localfeeddir}

git clone https://github.com/clicon/clixon-openwrt.git

cp -R clixon-openwrt/clixon/* ${localfeeddir}

rm -rf clixon-openwrt # remove the temporary git dir

# Create local feed for wifi application
test -d ${localfeeddir}/musenki || mkdir ${localfeeddir}/musenki
cp ${srcdir}/Makefile.musenki ${localfeeddir}/musenki/Makefile

# Create feeds config pointing to: standard packages and a local feed (generated above)
cat <<EOF > feeds.conf
src-git packages https://git.openwrt.org/feed/packages.git
src-link local ${localfeeddir}
EOF

echo "Updating feeds"
echo "=============="

./scripts/feeds update -a
./scripts/feeds update local
./scripts/feeds install -a -p local

cp ${localfeeddir}/clixon/files/950-netconf-subsystem.patch ${openwrtdir}/package/network/services/dropbear/patches/

# see https://openwrt.org/docs/guide-developer/toolchain/use-buildsystem#configure_using_config_diff_file

# Write changes to .config
# Create this by: ./scripts/diffconfig.sh > diffconfig (after make menuconfig; make download)
cat<<EOF > .config
CONFIG_TARGET_${TARGET}=y
CONFIG_TARGET_${TARGET}_${SUBTARGET}=y
CONFIG_TARGET_${TARGET}_${SUBTARGET}_DEVICE_${PROFILE}=y
CONFIG_OPENSSL_ENGINE=y
CONFIG_OPENSSL_OPTIMIZE_SPEED=y
CONFIG_OPENSSL_WITH_ASM=y
CONFIG_OPENSSL_WITH_CHACHA_POLY1305=y
CONFIG_OPENSSL_WITH_CMS=y
CONFIG_OPENSSL_WITH_DEPRECATED=y
CONFIG_OPENSSL_WITH_ERROR_MESSAGES=y
CONFIG_OPENSSL_WITH_PSK=y
CONFIG_OPENSSL_WITH_SRP=y
CONFIG_OPENSSL_WITH_TLS13=y
CONFIG_PACKAGE_cligen=y
CONFIG_PACKAGE_clixon=y
CONFIG_PACKAGE_libnghttp2=y
CONFIG_PACKAGE_libopenssl=y
CONFIG_PACKAGE_musenki=m
EOF

# Expand to full config

make -j${NCPUS} defconfig

echo "Download"
echo "========"
make download

echo "Make"
echo "===="
make -j${NCPUS}

if false; then # Already compiled but may be useful for incremental builds
    echo "Compile musenki app"
    echo "==================="
    
    make -j1 V=s package/musenki/download
    make -j1 V=s package/musenki/check V=s FIXUP=1
    make -j1 V=s package/musenki/compile
fi

echo "Build started at " ${STARTTIME}
echo -n "Build ended at "
date
