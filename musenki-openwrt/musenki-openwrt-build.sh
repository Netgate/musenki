#!/usr/bin/env bash
# Openwrt build script for musenki wifi app
set -eu

# Where build is made
: ${builddir=$(pwd)}
test -d ${builddir} || exit -1

# Where this script and dependent file reside
: ${srcdir:=$(pwd)}
test -d ${srcdir} || exit -1

# openwrt directory 
: ${openwrtdir=${builddir}/openwrt}
test -d ${srcdir} || exit -1

# This is the (created) local feed dir (where the wifi application is defined)
localfeeddir=${builddir}/tmplocalfeed

# Git openwrt verified tag (Jan 23 2022)
TAG=3d3d03479d5b4a976cf1320d29f4bd4937d5a4ba

function usage()
{
    echo "usage: $0"
    exit 255
}

if [ $# -ne 0 ]; then 
    usage
fi

cd ${openwrtdir}

if [ -n "${TAG}" ]; then 
    git checkout ${TAG}
fi

echo "Make distclean"
echo "=============="
make distclean

test ! -d ${localfeeddir} || rm -rf ${localfeeddir}
test -d ${localfeeddir} || mkdir ${localfeeddir}

# Dowwnload cligen/clixon default feed to a local feed
git clone https://github.com/clicon/clixon-openwrt.git

test -d ${localfeeddir}/cligen || mkdir ${localfeeddir}/cligen
cp clixon-openwrt/clixon/cligen/Makefile ${localfeeddir}/cligen/

test -d ${localfeeddir}/clixon || mkdir ${localfeeddir}/clixon
cp clixon-openwrt/clixon/clixon/Makefile ${localfeeddir}/clixon/

# Create local feed for wifi application
test -d ${localfeeddir}/wifi || mkdir ${localfeeddir}/wifi
cp ${srcdir}/Makefile.musenki ${localfeeddir}/wifi/Makefile

rm -rf clixon-openwrt # remove the temporary git dir

# This is taken from clixon-openwrt/clixon/clixon/Makefile

cat <<EOF > feeds.conf
src-git packages https://git.openwrt.org/feed/packages.git
src-link local ${localfeeddir}
EOF

echo "Updating feeds"
echo "=============="

./scripts/feeds update -a
./scripts/feeds update local
./scripts/feeds install -a -p local

# see https://openwrt.org/docs/guide-developer/toolchain/use-buildsystem#configure_using_config_diff_file

# Write changes to .config
# Create this by: ./scripts/diffconfig.sh > diffconfig (after make menuconfig; make download)
cat<<EOF > .config
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
# CONFIG_FEED_clixon is not set
# CONFIG_FEED_local is not set
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

make defconfig

#make clean

echo "Download"
echo "========"
make download

echo "Make"
echo "===="
make -j1 V=s # -j2 may break the machine?

echo "Compile musenki app"
echo "==================="

make package/musenki/download
make package/musenki/check V=s FIXUP=1
make -j1 V=s package/musenki/compile

sleep 1 # ensure OK is last
echo OK


