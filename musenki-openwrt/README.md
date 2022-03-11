# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt:
  * musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
  * musenki-vbox-install.sh: Start a virtualbox and install openwrt and the musenki application
  * Makefile.musenki: Local Openwrt feed makefile used in the build script

Build openwrt on x86-64 with clixon base and a musenki module:
```
git clone https://github.com/openwrt/openwrt.git
./musenki-openwrt-build.sh
```

As input, the script takes `TARGET`, `SUBTARGET` and `PROFILE`  corresponding to selections in openwrt `make menuconfig`. For example, x86-64-generic(default) and Raspberry PI4, respectively:
```
TARGET=x86 SUBTARGET=64 PROFILE=generic ./musenki-openwrt-build.sh
TARGET=bcm27xx SUBTARGET=bcm2711 PROFILE=rpi ./musenki-openwrt-build.sh
```

Create a virtualbox, install the image and load the musenki package:
```
./musenki-vbox-install.sh
...
Continue or ^C to keep VM?^C
```

Example run:
```
ssh root@192.168.1.1
root@OpenWrt:~# clixon_backend -f /etc/clixon.xml
root@OpenWrt:~# clixon_cli -f /etc/clixon.xml
wifi /> set access-point ?
  <hostname>            Access Point FQDN.                                                         
wifi /> set access-point 
```
