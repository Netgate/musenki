# musenki customization

You may find a need to customize musenki to add drivers or add tools
from the OpenWRT package lists, that process is described here.

1. To start, you will need to build musenki normally, following the directions in the QuickStart guide
2. When the build completes, change to the OpenWRT directory and start menuconfig:
```shell
cd openwrt
make menuconfig
```
3. Modify the configuration as you need and save/exit menuconfig.
4. Now to create the necessary configuration file (replace \<platform\>):
```shell
make defconfig
./scripts/diffconfig.sh > CONFIG-<platform>
```
5. copy the new CONFIG-\<platform\> to the musenki configs directory:
```shell
cp CONFIG-<platform> ../../configs
```
6. at this point you could optionally create a new image
```shell
make world
```
7. Finally, test the process by building musenki with the musenki-openwrt-build.sh script
8. If you like, please submit a pull request to incorporate your new config into the musenki repo.

