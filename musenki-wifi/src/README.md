# Clixon wifi openconfig example

  * [Content](#content)
  * [Compile and run](#compile)
  * [Using the CLI](#using-the-cli)
  * [Netconf](#netconf)	
  * [Restconf](#restconf)
  
## Content

This directory contains a Clixon openconfig wifi host example. It contains the following files:
* `wifi.xml` the XML configuration file
* `wifi_backend_plugin.c`: Backend plugin providing a glue to Openwrt UCI
* `wifi_cli.cli`: the CLIgen spec
* `startup_db`: The startup datastore containing restconf port configuration
* `Makefile.in`: where plugins are built and installed

## Host build

This description is for local development host builds. For proper target
compilation, use the [musenki openwrt build scripts](../../musenki-openwrt)

Before you start,
* Install [cligen and clixon](https://clixon-docs.readthedocs.io/en/latest/install.html)
* Setup [groups](https://github.com/clicon/clixon/blob/master/doc/FAQ.md#do-i-need-to-setup-anything)
* Checkout openconfig: git clone https://github.com/openconfig/public, this config assumes it to be checked out at: '/usr/local/share`

```
    make && sudo make install
```
Start backend in the background:
```
    sudo clixon_backend -f /usr/local/etc/wifi.xml -s startup
```
Note: use `-s init` instead if you want to start Clixon without the preconfigured restconf daemon

Start cli:
```
    clixon_cli -f /usr/local/etc/wifi.xml
```



