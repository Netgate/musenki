# Musenki Openwrt localfeed

This is a local feed dir of Clixon-based Musenki wifi application.
Add the following line to `feeds.conf` in the openwrt dir:
```
src-git-full packages https://git.openwrt.org/feed/packages.git
src-link local <dir>
```
where `<dir>`is the location of this dir.

Register the local feed as follows:
```
./scripts/feeds update -a
./scripts/feeds update local
./scripts/feeds install -a -p local
```

Thereafter run `make menuconfig`, include cligen and clixon from the utility section into the image (mark with `*`) and mark musenki as a module (`M`).

Finally make the image with `make`


