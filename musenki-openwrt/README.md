# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt
     - musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
     - musenki-vbox-install.sh: 
     - Makefile.musenki: Local Openwrt feed makefile used in the build script

Build musenki module:
```
git clone https://github.com/openwrt/openwrt.git
./musenki-openwrt-build.sh
```

Create a virtualbox, install musenki, and run it
```
./musenki-vbox-install.sh
```

Example run:
```
ssh root@192.168.1.1
root@OpenWrt:~# clixon_backend
root@OpenWrt:~# clixon_cli
```
