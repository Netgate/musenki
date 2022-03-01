# musenki

This project adds a CLI, RESTCONF, and NETCONF to OpenWRT using [clixon](https://github.com/clicon/clixon)

Contributions should use the [OpenConfig YANG models](https://www.openconfig.net/projects/models/) when available.
If an OpenConfig model is close, please use YANG `augment`. Completely new models will be scrutinized
and should match the OpenConfig models structure and style.

Directories:
- musenki-wifi - Wifi management plugin
- musenki-dhcp - DHCP management plugin
- musenki-openwrt - Build scripts for Musenki on openwrt

File:
- configure.c - should really be in musenki-wifi but openwrt/buildroot requires configure placed in root

