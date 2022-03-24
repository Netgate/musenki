# Running musenki on bhyve

musenki on bhyve is a prime target of Musenki

## determine target PCI device

Before you start, you need to determine the PCI slots you want
connected to the bhyve VM running OpenWRT.

Start off by executing "pciconf -l". This will display the list
of all PCI devices. In this example I have a board with 2 1Gb
NICs (re0 and re1). I want to give re1 to the VMD by setting
up a PCI Passthrough.

The initial output of pciconf:
```shell
dave@zima-1:~ % pciconf -l
hostb0@pci0:0:0:0:	class=0x060000 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5af0 subvendor=0x8086 subdevice=0x5af0
vgapci0@pci0:0:2:0:	class=0x030000 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5a85 subvendor=0x8086 subdevice=0x5a85
hdac0@pci0:0:14:0:	class=0x040300 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5a98 subvendor=0x1c6c subdevice=0x010e
none0@pci0:0:15:0:	class=0x078000 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5a9a subvendor=0x8086 subdevice=0x5a9a
ahci0@pci0:0:18:0:	class=0x010601 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5ae3 subvendor=0x8086 subdevice=0x5ae3
pcib1@pci0:0:19:0:	class=0x060400 rev=0xfb hdr=0x01 vendor=0x8086 device=0x5ad8 subvendor=0x8086 subdevice=0x5ad8
pcib2@pci0:0:20:0:	class=0x060400 rev=0xfb hdr=0x01 vendor=0x8086 device=0x5ad6 subvendor=0x8086 subdevice=0x5ad6
pcib3@pci0:0:20:1:	class=0x060400 rev=0xfb hdr=0x01 vendor=0x8086 device=0x5ad7 subvendor=0x8086 subdevice=0x5ad7
xhci0@pci0:0:21:0:	class=0x0c0330 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5aa8 subvendor=0x8086 subdevice=0x5aa8
sdhci_pci0@pci0:0:28:0:	class=0x080501 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5acc subvendor=0x8086 subdevice=0x5acc
isab0@pci0:0:31:0:	class=0x060100 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5ae8 subvendor=0x8086 subdevice=0x5ae8
ichsmb0@pci0:0:31:1:	class=0x0c0500 rev=0x0b hdr=0x00 vendor=0x8086 device=0x5ad4 subvendor=0x8086 subdevice=0x5ad4
re0@pci0:2:0:0:	class=0x020000 rev=0x15 hdr=0x00 vendor=0x10ec device=0x8168 subvendor=0x10ec subdevice=0x0123
re1@pci0:3:0:0:	class=0x020000 rev=0x15 hdr=0x00 vendor=0x10ec device=0x8168 subvendor=0x10ec subdevice=0x0123
```
Note the entry for re1:
```shell
re1@pci0:3:0:0:	class=0x020000 rev=0x15 hdr=0x00 vendor=0x10ec device=0x8168 subvendor=0x10ec subdevice=0x0123
```
The PCI bus address in this is "re1@pci0:3:0:0" - the digits after the first colon are the
address you need. In this case "3:0:0". The syntax for the PCI address used in the next step
is formed by replacing the ":" with "/" to produce "3/0/0". You will use this in the next step.

## Modify /boot/loader.conf

Add the followinfg lines to /boot/loader.conf:
```shell
vmm_load="YES"
pptdevs="3/0/0"
```
This causes the loading of vmm, the kernel module for bhyve, and tells the kernel to assign
the ppt (PCI passthrough) driver to PCI bus address 3/0/0.

Next, reboot the host FreeBSD system. You should now see re1 gone, and "pciconf -l" shows 3/0/0
attached to the ppt driver:
```shell
ppt0@pci0:3:0:0:	class=0x020000 rev=0x15 hdr=0x00 vendor=0x10ec device=0x8168 subvendor=0x10ec subdevice=0x0123
```
If you don't get this, ask for help - these instructions are empirical and have not
been widely tested.

## TODOs
- how to start up the VM manually?
- configure for start at boot time
