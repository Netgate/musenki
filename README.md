# musenki

This project adds a CLI, RESTCONF, and NETCONF to OpenWRT using [clixon](https://github.com/clicon/clixon). 

Contributions should use the [OpenConfig YANG models](https://www.openconfig.net/projects/models/) when available.
If an OpenConfig model is close, please use YANG `augment`. Completely new models will be scrutinized
and should match the OpenConfig models structure and style.

Directories:
- musenki-wifi - Wifi management plugin
- musenki-dhcp - DHCP management plugin
- musenki-openwrt - Build scripts for Musenki on openwrt

## Quickstart

Build openwrt for RPI4 with clixon base and a musenki module:
```
cd musenki-openwrt
git clone https://github.com/openwrt/openwrt.git
TARGET=bcm27xx SUBTARGET=bcm2711 PROFILE=rpi ./musenki-openwrt-build.sh
```

If you prefer a manual build, follow the [manual build instructions](musenki-openwrt/musenkilocalfeed/README.md).

Flash a SD with an openwrt image:
```
cd openwrt//bin/targets/bcm27xx/bcm2711
gunzip openwrt-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz
sudo dd if=openwrt-bcm27xx-bcm2711-rpi-4-squashfs-factory.img of=/dev/mmcblk0 bs=2M conv=fsync
```

Boot the RPI4, set IP addresses, etc (here target is at `root@pi4.hagsand.com`).
Copy the Musenki package to target, install it and start clixon:
```
cd openwrt/bin/packages/aarch64_cortex-a72/local/
scp musenki_HEAD-1_aarch64_cortex-a72.ipk root@pi4.hagsand.com:
ssh root@pi4.hagsand.com
opkg install ./musenki_HEAD-1_aarch64_cortex-a72.ipk
sudo clixon_backend -s startup
```

Configure wifi:
```
root@pi4:~# clixon_cli
wifi /> set provision-ap E4:5F:01:6F:3D:7B mac E4:5F:01:6F:3D:7B
wifi /> set provision-ap E4:5F:01:6F:3D:7B hostname pi4         
wifi /> set provision-ap E4:5F:01:6F:3D:7B country-code US
wifi /> mode access-point pi4
wifi access-point=pi4/> set hostname pi4
wifi access-point=pi4/> mode radio 0 oc-wifi-types:FREQ_5GHZ 
wifi radio=0,oc-wifi-types%3AFREQ_5GHZ/> set id 0
wifi radio=0,oc-wifi-types%3AFREQ_5GHZ/> set operating-frequency oc-wifi-types:FREQ_5GHZ
wifi radio=0,oc-wifi-types%3AFREQ_5GHZ/> up
wifi access-point=pi4/> set ssid Grendel name Grendel
wifi access-point=pi4/> set ssid Grendel wpa2-psk my-passwd
wifi access-point=pi4/> top
wifi /> commit
```

Check underlying config, status and logs:
```
root@pi4:~# cat /etc/config/wireless
root@pi4:~# wifi status
root@pi4:~# logread
```

## Netconf

Send local netconf command:
```
root@pi4:~# clixon_netconf -q
  <?xml version="1.0" encoding="UTF-8"?>
  <hello xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
     <capabilities><capability>urn:ietf:params:netconf:base:1.1</capability></capabilities>
   </hello>]]>]]>
  <rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="0"><get-config><source><running/></source></get-config></rpc>]]>]]>
```

## Restconf

Send restconf commands using curl:
```
curl -Ssik -X GET http://pi4.hagsand.com:8080/restconf/data
```
