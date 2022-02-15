# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt
     - musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
     - musenki-vbox-install.sh: 
     - Makefile.musenki: Local Openwrt feed makefile used in the build script

Build a openwrt musenki module:
```
git clone https://github.com/openwrt/openwrt.git
./musenki-openwrt-build.sh
```

Create an x86_64 virtualbox and install the musenki package:
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
