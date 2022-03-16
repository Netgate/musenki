# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt:
  * musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
  * musenki-vbox-install.sh: Start a virtualbox and install openwrt and the musenki application
  * Makefile.musenki: Local Openwrt feed makefile used in the build script (to generate musenki local feed)
  * musenkilocalfeed: Snapshot of local openwrt feed for building clixon and musenki.

The build script creates a local feed at used by openwrt to install
Clixon and Musenki. Note that the build script creates the script as a temporary directory. There is also a snapshot which you can use for manual builds.

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

For manual openwrt build, see [local feed description](musenkilocalfeed).

Create a virtualbox, install the image and load the musenki package:
```
./musenki-vbox-install.sh
...
Continue or ^C to keep VM?^C
```

Re-compile the musenki package:
```
	(cd openwrt; make -j1 V=s package/musenki/compile)
```

See [quickstart](../README.md#quickstart) for how to config an example musenki AP