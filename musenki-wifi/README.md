# Clixon wifi application

* `musenki.xml` the XML configuration file of the application holding Clixon options
* `autocli.xml` the XML configuration file for the CLI
* `restconf.xml` the XML configuration file for the RESTCONF backend
* `nacm.xml` the XML configuration file for the NETCONF backend
* [Wifi plugins](wifi)
* [System plugins](system)

## Host build

This description is for local development host builds. For proper target
compilation, use the [musenki openwrt build scripts](../musenki-openwrt)

Before you start,
* Install [cligen and clixon](https://clixon-docs.readthedocs.io/en/latest/install.html)
* Setup [groups](https://github.com/clicon/clixon/blob/master/doc/FAQ.md#do-i-need-to-setup-anything)
* Checkout openconfig: git clone https://github.com/openconfig/public, this config assumes it to be checked out at: '/usr/local/share`

```
    make && sudo make install
```
Start backend in the background:
```
    sudo clixon_backend -f /usr/local/etc/clixon/musenki.xml -s startup
```
Note: use `-s init` instead if you want to start Clixon without the preconfigured restconf daemon

Start cli:
```
    clixon_cli -f /usr/local/etc/clixon/musenki.xml
```
