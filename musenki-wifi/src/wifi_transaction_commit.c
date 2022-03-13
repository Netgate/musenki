/* -*- c-basic-offset: 4; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>
#include <sys/utsname.h>

#include <cligen/cligen.h>
#include <clixon/clixon.h>
#include <clixon/clixon_backend.h>
#include <clixon/clixon_xml.h>
#include <clixon/clixon_xpath.h>

#include "uci_interface.h"

#include "wifi.h"

/* Access paths for openconfig wifi data
 * Notes: 
 *  1) There are prefixes, with proper default prefixing one could do without them
 *  2) There are no keys, maybe those are necessary, eg provision-ap[mac='0:0:0:0:0:0:']
 */
/* See openconfig-ap-manager.yang */
#define PROV_PATH "/ap-manager:provision-aps/ap-manager:provision-ap/ap-manager:config"
/* See openconfig-access-points.yang */
#define AP_PATH "/access-points:access-points/access-points:access-point"
/* See openconfig-wifi-mac.yang */
#define SSID_PATH AP_PATH "/access-points:ssids/access-points:ssid/access-points:config"
/* See openconfig-wifi-phy.yang */
#define RADIO_PATH AP_PATH "/access-points:radios/access-points:radio"

/*! Get path to radio phy 
 * Specifies the radio phy associated to this section. If present, it is usually 
 * autodetected and should not be changed. 
 * Alternative to phy used to identify the device based paths in /sys/devices
 * @param[in]   h        Clixon handle
 * @param[out]  phy_path 
 * XXX: currently this is ad-hoc, how to do this properly?
 */
static int
wifi_phy_path(clicon_handle h,
	      char        **phy_path)
{
    int retval = -1;
    struct utsname un = {0,};

    if (phy_path == NULL){
	clicon_err(OE_PLUGIN, EINVAL, "phy_path is NULL");
	goto done;
    }
    if (uname(&un) < 0){
	clicon_err(OE_PLUGIN, errno, "uname");
	goto done;
    }
    if (strcmp(un.machine, "aarch64") == 0) /* PI? */
	*phy_path = "platform/soc/fe300000.mmcnr/mmc_host/mmc1/mmc1:0001/mmc1:0001:1";
    else
    	*phy_path = "platform/soc/fe300000.mmcnr/mmc_host/mmc1/mmc1:0001/mmc1:0001:1";
    retval = 0;
 done:
    return retval;
}

/*! Set openwrt wifi physical radio devices present on the system
 * @see https://openwrt.org/docs/guide-user/network/wifi/basic#wi-fi_devices
 */
int
wifi_device(clicon_handle h,
	    cvec         *nsc,
	    cxobj        *xmlconfig)
{
    int    retval = -1;
    cxobj *cx;
    char  *opfreq = NULL;
    char  *hwmode = NULL;
    char  *phy_path = NULL;
    char  *channel = "auto";
    cbuf  *htmode = NULL;
    char  *country = NULL;
    char  *enabled = "false";

    /* get phy-path (auto-detected) */
    if (wifi_phy_path(h, &phy_path) < 0)
	goto done;
    if ((cx = xpath_first (xmlconfig, nsc, RADIO_PATH "/access-points:config/access-points:channel")) != NULL)
	channel = xml_body(cx);
    
    if ((cx = xpath_first (xmlconfig, nsc, RADIO_PATH "/access-points:operating-frequency")) != NULL &&
	(opfreq = xml_body(cx)) != NULL){
	if (strcmp(opfreq, "oc-wifi-types:FREQ_2GHZ") == 0)
	    hwmode = "11g";
	else if (strcmp(opfreq, "oc-wifi-types:FREQ_2_5GHZ") == 0)
	    ; /* dual-mode ? */
	else if (strcmp(opfreq, "oc-wifi-types:FREQ_5GHZ") == 0)
	    hwmode = "11a";
    }
    if (hwmode == NULL){
	clicon_log (LOG_NOTICE, "%s: hwmode is NULL, problem?", __func__);
	goto ok;
    }
    if ((cx = xpath_first (xmlconfig, nsc, RADIO_PATH "/access-points:config/access-points:channel-width")) != NULL){
	if ((htmode = cbuf_new()) == NULL){
	    clicon_err(OE_UNIX, errno, "cbuf_new");
	    goto done;
	}
	cprintf(htmode, "VHT%s", xml_body(cx));
    }
    if (htmode == NULL){
	clicon_log (LOG_NOTICE, "%s: htmode is NULL, problem?", __func__);
	goto ok;
    }
    if ((cx = xpath_first (xmlconfig, nsc, PROV_PATH "/ap-manager:country-code")) != NULL){
	country = xml_body(cx);
    }
    if (country == NULL){
	clicon_log (LOG_NOTICE, "%s: country is NULL, problem?", __func__);
	goto ok;
    }
    if ((cx = xpath_first (xmlconfig, nsc, RADIO_PATH "/access-points:config/access-points:enabled")) != NULL){
	enabled = xml_body(cx);
    }
    if (uci_set("wireless.radio0", "wifi-device", NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.type", "mac80211", NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.channel", channel, NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.hwmode", hwmode, NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.path", phy_path, NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.htmode", cbuf_get(htmode), NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.country", country, NULL) < 0)
	goto done;
    if (uci_set("wireless.radio0.disabled", (strcmp(enabled,"false")==0)?"1":"0", NULL) < 0)
	goto done;
 ok:
    retval = 0;
 done:
    if (htmode)
	cbuf_free(htmode);
    return retval;
}

/*! Set openwrt wifi wireless network on top of the hardware
 * @see https://openwrt.org/docs/guide-user/network/wifi/basic#wi-fi_interfaces
 */
int
wifi_interface(clicon_handle h,
	       cvec *nsc,
	       cxobj *xmlconfig)
{
    int    retval = -1;
    cxobj *cx;
    char  *ssid = NULL;
    char  *key;
    char  *opmode = NULL;
    char  *encryption;

    /* See openconfig-wifi-phy.yang 
     * and https://openwrt.org/docs/guide-user/network/wifi/basic#encryption_modes
     */
    if ((cx = xpath_first (xmlconfig, nsc, SSID_PATH "/access-points:opmode")) != NULL){
	opmode = xml_body(cx);
	if (strcmp(opmode, "OPEN")==0)
	    ;
	else if (strcmp(opmode, "WPA2_PERSONAL")==0)
	    encryption = "psk2";
	else if (strcmp(opmode, "WPA2_ENTERPRISE")==0)
	    ;
	else if (strcmp(opmode, "WPA_PERSONAL")==0)
	    ;
	else if (strcmp(opmode, "WPA_ENTERPRISE")==0)
	    ;
	else if (strcmp(opmode, "WEP")==0)
	    ;
    }
    if (encryption == NULL){
	clicon_log (LOG_NOTICE, "%s: encryption is NULL, problem?", __func__);
	goto ok;
    }
    if ((cx = xpath_first (xmlconfig, nsc, SSID_PATH "/access-points:name")) != NULL)
	ssid = xml_body(cx);
    if (ssid == NULL){
	clicon_log (LOG_NOTICE, "%s: ssid is NULL, problem?", __func__);
	goto ok;
    }
    clicon_log (LOG_NOTICE, "%s: ssid: '%s'", __func__, ssid);
    if ((cx = xpath_first (xmlconfig, nsc, SSID_PATH "/access-points:wpa2-psk")) != NULL)
	key = xml_body(cx);	
    if (key == NULL) {
	clicon_log (LOG_NOTICE, "%s: key is NULL, problem?", __func__);
	return 0;
    }
    clicon_log (LOG_NOTICE, "%s: key: '%s'", __func__, key);

    if (uci_set ("wireless.default_radio0", "default_radio0", NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.device", "radio0", NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.network", "lan", NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.mode", "ap", NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.ssid", ssid, NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.encryption", encryption, NULL) < 0)
	goto done;
    if (uci_set ("wireless.default_radio0.key", key, NULL) < 0)
	goto done;
 ok:
    retval = 0;
 done:
    return retval;
}

int
wifi_transaction_commit(clicon_handle    h,
			transaction_data td)
{
    int   retval = -1;
    cxobj *xmlconfig;
    cvec *nsc;
    cxobj *cx;
    char *hostname;
    
    clicon_log (LOG_NOTICE, "called %s", __func__);

    /* XXX NEED TO FILTER OTHER COMMITS OUT */
    
    nsc = clicon_nsctx_global_get (h);
    if (nsc == NULL) {
	/* crash protection */
	clicon_err (OE_FATAL, 0, "bad global namespace context");
	goto done;
    }

    xmlconfig = transaction_target (td);
    if (xmlconfig == NULL) {
	/* crash protection */
	clicon_err (OE_FATAL, 0, "bad target DB pointer");
	goto done;
    }
    
    /* get hostname */
    if ((cx = xpath_first (xmlconfig, nsc, PROV_PATH "/ap-manager:hostname")) != NULL)
	hostname = xml_body(cx);
    if (hostname == NULL)
	goto ok; 	/* nothing defined, bail happy */
    clicon_log (LOG_NOTICE, "%s: hostname: '%s'", __func__, hostname);
    
    /* delete the existing configuration */
    if (uci_delete ("wireless.radio0", NULL, NULL) < 0)
	goto done;
    if (uci_delete ("wireless.default_radio0", NULL, NULL) < 0)
	goto done;

    /* make the wifi device */
    if (wifi_device(h, nsc, xmlconfig) < 0)
	goto done;

    /* make the wifi interface */
    if (wifi_interface(h, nsc, xmlconfig) < 0)
	goto done;

    /* make it official */
    if (uci_commit ("wireless", NULL) < 0)
	goto done;
    
    /* XXX wifi reload? */

 ok:
    retval = 0;
 done:
    return retval;
}
