# Musenki openwrt scripts

This dir contains scripts for building and installing Musenki using clixon and openwrt:
  * musenki-openwrt-build.sh: Download and build Musenki WIFI app for x86_64
  * musenki-vbox-install.sh: Start a virtualbox and install openwrt and the musenki application
  * Makefile.musenki: Local Openwrt feed makefile used in the build script (to generate musenki local feed)
  * musenkilocalfeed: Snapshot of local openwrt feed for building clixon and musenki.

The build script creates a local feed at used by openwrt to install
Clixon and Musenki. Note that the build script creates the script as a temporary directory. There is also a snapshot which you can use for manual builds.

See [quickstart](../docs/Quickstart.md#quickstart) for instructions of how to builr musenki.
