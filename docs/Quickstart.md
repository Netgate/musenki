# Quickstart

Build Musenki for Raspberry Pi 4
```
git clone https://github.com/Netgate/musenki.git
cd musenki/musenki-openwrt
git clone https://github.com/openwrt/openwrt.git
./musenki-openwrt-build.sh ../configs/CONFIG-RPI4
```

If you prefer a manual build, follow the [manual build instructions](musenki-openwrt/musenkilocalfeed/README.md).

Flash a SD with an openwrt image:
```
cd openwrt/bin/targets/bcm27xx/bcm2711
gunzip openwrt-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz
sudo dd if=openwrt-bcm27xx-bcm2711-rpi-4-squashfs-factory.img of=/dev/mmcblk0 bs=2M conv=fsync
```

Boot the RPI4, set IP addresses, etc (here target is at `root@192.168.1.1`).
Copy the Musenki package to target, install it and start clixon:
```
cd openwrt/bin/packages/aarch64_cortex-a72/local/
scp musenki_HEAD-1_aarch64_cortex-a72.ipk root@192.168.1.1:
ssh root@192.168.1.1
opkg install ./musenki_HEAD-1_aarch64_cortex-a72.ipk
useradd -M -U clicon
useradd www-data -g clicon
/etc/init.d/clixon start
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

## Virtualbox

As an alternative to flashing and running on a physical box, you can use virtualbox:
```
cd musenki-openwrt
TARGET=x86 SUBTARGET=64  ./musenki-openwrt-build.sh
./musenki-vbox-install.sh
```

## Netconf

Send netconf command using ssh:
```
ssh -s root@192.168.1.1 netconf
  <?xml version="1.0" encoding="UTF-8"?>
  <hello xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
     <capabilities><capability>urn:ietf:params:netconf:base:1.1</capability></capabilities>
   </hello>]]>]]>
  <rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="0"><get-config><source><running/></source></get-config></rpc>]]>]]>
```

## Restconf

The musenki-wifi has an example RESTCONF configuration, needs keys and certs under /etc/ssl.
Edit the startup-db to change them.
Example with generated (self-signed) certs:
```
scp srv_key.pem root@192.168.1.1:/etc/ssl/private/
scp srv_cert.pem  root@192.168.1.1:/etc/ssl/certs/
scp ca_cert.pem  root@192.168.1.1:/etc/ssl/certs/
ssh root@192.168.1.1 
root@pi4:~# service clixon restart
```

Send restconf commands using curl:
```
curl -k -X GET --key ./guest.key --cert ./guest.crt https://192.168.1.1/restconf/data
```
