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

(Re-)compile the musenki package:
```
	(cd openwrt; make -j1 V=s package/musenki/compile)
```

Copy the musenki-wifi package, install and run on target (RPI4 example)
XXX: Also copy a ready-made startup-db (more info here):
```
scp openwrt/bin/packages/aarch64_cortex-a72/local/musenki_HEAD-1_aarch64_cortex-a72.ipk root@pi4.hagsand.com:
# XXX Copy correct startup (or edit config)
scp startup_db root@pi4.hagsand.com:/usr/var/wifi/
ssh root@pi4.hagsand.com
opkg install ./musenki_HEAD-1_aarch64_cortex-a72.ipk
sudo clixon_backend -s startup
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
