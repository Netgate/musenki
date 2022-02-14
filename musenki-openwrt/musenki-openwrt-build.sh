#!/usr/bin/env bash
# Openwrt build script for musenki wifi app
# Example:
#  cd <targetdir>
#  srcdir=~/src/musenki/musenki-openwrt ~/src/musenki/musenki-openwrt/build-openwrt-musenki.sh distclean
#  srcdir=~/src/musenki/musenki-openwrt build-openwrt-musenki.sh distclean
# May need prereqs, eg
#   sudo apt install libncurses-dev
set -eu

# Where build is made
: ${builddir=$(pwd)}
test -d ${builddir} || exit -1

# Where this script and dependent file reside
: ${srcdir:=$(pwd)}
test -d ${srcdir} || exit -1

# Git openwrt verified tag (Jan 23 2022)
TAG=3d3d03479d5b4a976cf1320d29f4bd4937d5a4ba

# openwrt directory 
: ${openwrtdir=${builddir}/openwrt}
test -d ${srcdir} || exit -1

# This is the (created) local feed dir (where the wifi application is defined)
localfeeddir=${builddir}/tmplocalfeed

IMAGE=openwrt-x86-64-generic-ext4-combined.img
sNAME="openwrtx64-clixon"
VDI="${sNAME}.vdi"
VMNAME="${sNAME}"
DISKSIZE='512000000'
#VMFOLD="`cat ~/.VirtualBox/VirtualBox.xml | grep -i SystemProperties | cut -d'"' -f2`"
VMFOLD="/home/olof/VirtualBox\ VMs"
IMGC="${openwrtdir}/bin/targets/x86/64/${IMAGE}.gz"

function usage()
{
    echo "usage: 
       distclean : build openwrt from scratch + start vm and run test
       vbox      : start vm and run *stest
       test      : run test"
}

if [ $# -ne 1 ]; then 
    usage
    exit 255
fi

# Verb: what to do
what=$1
if [ $what != distclean -a $what != vbox -a $what != test ]; then
    usage
    exit 255
fi

if [ ${what} = checkout ] ; then
    echo "Cloning openwrt"
    echo "=============="
    rm -rf ${openwrtdir}
    git clone https://github.com/openwrt/openwrt.git
fi

test -d ${openwrtdir} || exit -1
cd ${openwrtdir}

if [ -n "${TAG}" ]; then 
    git checkout ${TAG}
fi

if [ ${what} = distclean ] ; then
    echo "Make distclean"
    echo "=============="
    make distclean
fi

if [ $what != vbox -a $what != test ]; then

test ! -d ${localfeeddir} || rm -rf ${localfeeddir}
test -d ${localfeeddir} || mkdir ${localfeeddir}
# Copy git feeds into local feeds.
# Alternative is to copy them inline here
git clone https://github.com/clicon/clixon-openwrt.git

# Replace version with HEAD
test -d ${localfeeddir}/cligen || mkdir ${localfeeddir}/cligen
cp clixon-openwrt/clixon/cligen/Makefile ${localfeeddir}/cligen/
#sed -i 's/PKG_VERSION:=.*/PKG_VERSION:=HEAD/g' ${localfeeddir}/cligen/Makefile

test -d ${localfeeddir}/clixon || mkdir ${localfeeddir}/clixon
cp clixon-openwrt/clixon/clixon/Makefile ${localfeeddir}/clixon/
#sed -i 's/PKG_VERSION:=.*/PKG_VERSION:=HEAD/g' ${localfeeddir}/clixon/Makefile
#sed -i 's/--disable-evhtp//g' ${localfeeddir}/clixon/Makefile

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
CONFIG_PACKAGE_wifi=m
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

fi # vbox

if [ ${what} != test ] ; then

echo "    Creating VM: $VMNAME"
echo "          Cache: ${HOME}/cache"
echo "       Disksize: $DISKSIZE"

VBoxManage controlvm ${VMNAME} poweroff || true
sleep 2
VBoxManage unregistervm --delete ${VMNAME} || true

VBoxManage createvm --name $VMNAME --register

VBoxManage modifyvm $VMNAME \
    --description "openwrt vbox" \
    --ostype "Linux26" \
    --memory "512" \
    --cpus "1" \
    --hostonlyadapter1="vboxnet0" \
    --nic1 "hostonly" \
    --nictype1 82540EM \
    --cableconnected1="on" \
    --macaddress2="08002730B510" \
    --cableconnected2="on" \
    --nic2="nat" \
    --nictype2="82540EM"

# Host side:
vboxmanage hostonlyif ipconfig vboxnet0 --ip 192.168.1.2

VBoxManage storagectl $VMNAME \
    --name "SATA Controller" \
    --add "sata" \
    --portcount "4" \
    --hostiocache "on" \
    --bootable "on" && \

rm -f ${VDI}
    
gunzip --stdout "${IMGC}" | VBoxManage convertfromraw --format VDI stdin "${VDI}" $DISKSIZE 

#mv "${VDI}" "${VMFOLD}/${VMNAME}"
#test -d " || mkdir "${VMFOLD}/${VMNAME}" || true
cp ${VDI} /home/olof/VirtualBox\ VMs/$VMNAME/ # XXX cant get space right

VBoxManage storageattach $VMNAME \
    --storagectl "SATA Controller" \
    --port "1" \
    --type "hdd" \
    --nonrotational "on" \
    --medium /home/olof/VirtualBox\ VMs/${VMNAME}/$VDI

VBoxManage startvm ${VMNAME} --type headless

sleep 20

fi # test

ssh-keygen -f "/home/olof/.ssh/known_hosts" -R "192.168.1.1"

exit
cat<<EOF > $openwrtdir/clixontest.sh
#!/usr/bin/env sh
set -eux
opkg update
opkg install git-http shadow-useradd
useradd -M -U clicon || true
useradd www-data -g clicon || true

# Always clone a new clixon
#if [ -d clixon ]; then
#   (cd clixon; git pull)
#else
#   git clone https://github.com/clicon/clixon.git
#fi
# packages for clixon smoketest tests
opkg install curl coreutils-od sudo diffutils procps-ng-pkill bash
EOF
chmod 755 $openwrtdir/clixontest.sh

cat<<EOF > $openwrtdir/config.sh
YANG_INSTALLDIR=/usr/share/clixon
EOF
chmod 755 $openwrtdir/clixontest.sh

ssh-keygen -f "/home/olof/.ssh/known_hosts" -R "192.168.1.1"
scp -p -o StrictHostKeyChecking=no $openwrtdir/clixontest.sh root@192.168.1.1:.
ssh -o StrictHostKeyChecking=no root@192.168.1.1 ./clixontest.sh

scp -p -o StrictHostKeyChecking=no $openwrtdir/config.sh root@192.168.1.1:clixon/test/

# run test
#ssh -o StrictHostKeyChecking=no root@192.168.1.1 "(cd clixon/test; ./test_helloworld.sh)"

# Close VM unless only run test
if [ ${what} != test ] ; then
    read -n 1 -p "Continue or ^C to keep VM?"

    VBoxManage controlvm ${VMNAME} poweroff
    sleep 2
    VBoxManage unregistervm --delete ${VMNAME}
fi

sleep 1 # ensure OK is last
echo OK


