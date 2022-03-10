#!/bin/sh -x

# This script configures a bhyve contqiner and boots musenki

# Initial setup of interfaces and modules
kldunload vmm.ko
kenv hw.vmm.vmx.use_apic_vid=0
kldload vmm.ko
ifconfig tap0 create
sysctl net.link.tap.up_on_open=1
ifconfig tap0 inet 10.1.1.1 netmask 255.255.255.0

# Add commands to start the VM in a tmux session
export LC_ALL=en_US.UTF-8

#cd /root/wifi

vm_name="openwrtguest"
vm_ram="512M"
vm_cpus="2"
vm_args1="-S -A -H -P"
vm_args2="-c ${vm_cpus} -m ${vm_ram}"
vm_devs="-s 0:0,hostbridge -s 1:0,lpc"
vm_net0="-s 2:0,virtio-net,tap0"
#vm_net1="-s 4:0,virtio-net,tap1"
vm_disk0="-s 3:0,virtio-blk,./openwrt.img"
vm_passthru="-s 7,passthru,17/0/0"
vm_console="-l com1,stdio"

bhyvectl --destroy --vm=openwrtguest

grub-bhyve -m device.map -S -r hd0,msdos1 -M $vm_ram $vm_name
tmux new -s vm "bhyve $vm_args1 $vm_devs $vm_net0 $vm_net1 $vm_disk0 $vm_passthru $vm_console $vm_args2 $vm_name"

