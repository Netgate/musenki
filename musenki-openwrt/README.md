# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt
     - musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
     - musenki-vbox-install.sh: 
     - Makefile.musenki: Local Openwrt feed makefile used in the build script

Build musenki module in `<targetdir>`, This dir is `<srcdir>`. They can be the same dir:
```
cd <targetdir>
git clone https://github.com/openwrt/openwrt.git
srcdir=<srcdir> <srcdir>/build-openwrt-musenki.sh distclean

make package/wifi/download
make package/wifi/check V=s FIXUP=1
make -j1 V=s package/wifi/compile
```

Copy and install:
```
scp bin/packages/x86_64/local/wifi_HEAD-1_x86_64.ipk root@192.168.1.1:/tmp
ssh root@192.168.1.1
root@OpenWrt:~# opkg install /tmp/cligen_5.2.0-1_x86_64.ipk
```

Run:
```
clixon_backend -f /etc/wifi.xml
```
