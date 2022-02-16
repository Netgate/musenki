#!/usr/bin/env bash
# Create a virtualbox for x86_64 and install musenki wifi app
set -eux

# Where build is made
: ${builddir=$(pwd)}
test -d ${builddir} || exit -1

# openwrt directory 
: ${openwrtdir=${builddir}/openwrt}
test -d ${openwrtdir} || exit -1

IMAGE=openwrt-x86-64-generic-ext4-combined.img
sNAME=openwrtx64-clixon
VDI="${sNAME}.vdi"
VMNAME="${sNAME}"
DISKSIZE='512000000'
# Hardcoded to x86_64
IMGC="${openwrtdir}/bin/targets/x86/64/${IMAGE}.gz"
PKG=musenki_HEAD-1_x86_64.ipk
PKGPATH="${openwrtdir}/bin/packages/x86_64/local/${PKG}"
IPADDR=192.168.1.1 # Default virtualbox host

function usage()
{
    echo "usage: $0"
    exit 255
}

if [ $# -ne 0 ]; then 
    usage
fi

cd ${openwrtdir}

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

cp ${VDI} /home/olof/VirtualBox\ VMs/$VMNAME/ # XXX cant get space right

VBoxManage storageattach $VMNAME \
    --storagectl "SATA Controller" \
    --port "1" \
    --type "hdd" \
    --nonrotational "on" \
    --medium /home/olof/VirtualBox\ VMs/${VMNAME}/$VDI

VBoxManage startvm ${VMNAME} --type headless

ssh-keygen -f "/home/olof/.ssh/known_hosts" -R "${IPADDR}"

echo -n "Waiting for VM to boot"
for (( i=1; i<10; i++ )); do  
    echo -n "."
    sleep 1
    ssh -o StrictHostKeyChecking=no root@${IPADDR} pwd > /dev/null
done

# Install musenki package
scp -o StrictHostKeyChecking=no ${PKGPATH} root@${IPADDR}:/tmp
ssh -o StrictHostKeyChecking=no root@${IPADDR} opkg install "/tmp/${PKG}"

# Clixon setup things on vm
cat<<EOF > ${builddir}/setup.sh
#!/usr/bin/env sh
set -eux
opkg update
opkg install shadow-useradd
useradd -M -U clicon || true
useradd www-data -g clicon || true
EOF
chmod 755 ${builddir}/setup.sh

scp -p -o StrictHostKeyChecking=no ${builddir}/setup.sh root@${IPADDR}:.
ssh -o StrictHostKeyChecking=no root@${IPADDR} ./setup.sh

# Close VM unless only run test
read -n 1 -p "Continue or ^C to keep VM?"

VBoxManage controlvm ${VMNAME} poweroff
sleep 2
VBoxManage unregistervm --delete ${VMNAME}

sleep 1 # ensure OK is last
echo OK


