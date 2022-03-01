/* -*- c-basic-offset: 4; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include <cligen/cligen.h>
#include <clixon/clixon.h>
#include <clixon/clixon_backend.h>
#include <clixon/clixon_xml.h>
#include <clixon/clixon_xpath.h>

#include "uci_interface.h"

#include "wifi.h"

#define HOSTNAME_PATH "/ap-manager:provision-aps/ap-manager:provision-ap/ap-manager:config/ap-manager:hostname"
#define SSID_PATH "/access-points:access-points/access-points:access-point/access-points:ssids/access-points:ssid/access-points:config/access-points:name"
#define ENCRYPTION_PATH "/access-points:access-points/access-points:access-point/access-points:ssids/access-points:ssid/access-points:config/access-points:opmode"
#define KEY_PATH "/access-points:access-points/access-points:access-point/access-points:ssids/access-points:ssid/access-points:config/access-points:wpa2-psk"
#define STATUS_PATH "/access-points:access-points/access-points:access-point/access-points:ssids/access-points:ssid/access-points:config/access-points:enabled"

int wifi_transaction_commit(clicon_handle h, transaction_data td)
{
    cbuf *result;
    int status;
    cxobj *target_db;
    cvec *nsc;
    
    clicon_log (LOG_NOTICE, "called %s", __func__);

    /* NEED TO FILTER OTHER COMMITS OUT */
    
    nsc = clicon_nsctx_global_get (h);
    if (nsc == NULL) {
	/* crash protection */
	clicon_err (OE_FATAL, 0, "bad global namespace context");
    }

    target_db = transaction_target (td);
    if (target_db == NULL) {
	/* crash protection */
	clicon_err (OE_FATAL, 0, "bad target DB pointer");
    }
    
    /* get hostname */
    cxobj *cx_hostname = xpath_first (target_db, nsc, HOSTNAME_PATH);
    char *hostname = xml_body (cx_hostname);
    
    if (hostname == NULL) {
	/* nothing defined, bail happy */
	return 0;
    }
    clicon_log (LOG_NOTICE, "%s: hostname: '%s'", __func__, hostname);
    
    /*get SSID */
    cxobj *cx_ssid = xpath_first (target_db, nsc, ENCRYPTION_PATH);
    char *ssid = xml_body (cx_ssid);

    if (ssid == NULL) {
	clicon_log (LOG_NOTICE, "%s: ssid is NULL, problem?", __func__);
	return 0;
    }
    clicon_log (LOG_NOTICE, "%s: ssid: '%s'", __func__, ssid);
    
    /* get encryption */
    cxobj *cx_encryption = xpath_first (target_db, nsc, KEY_PATH);
    char *encryption = xml_body (cx_encryption);

    if (encryption == NULL) {
	clicon_log (LOG_NOTICE, "%s: encryption is NULL, problem?", __func__);
	return 0;
    }
    clicon_log (LOG_NOTICE, "%s: ssid: '%s'", __func__, encryption);

    /* these need implementing */
    char *channel = "36";
    char *hwmode = "11a";
    
    /* delete the existing configuration */
    status = uci_delete ("wireless.radio0", NULL, result);
    status = uci_delete ("wireless.default_radio0", NULL, result);

    /* make the wifi device */
    status = uci_set ("wireless.radio0", "wifi-device", NULL);
    status = uci_set ("wireless.radio0.type", "mac80211", NULL);
    status = uci_set ("wireless.radio0.channel", channel, NULL);
    status = uci_set ("wireless.radio0.hwmode", hwmode, NULL);
    status = uci_set ("wireless.radio0.path", "pci0000:00/0000:00:07.0", NULL);
    status = uci_set ("wireless.radio0.htmode", "VHT80", NULL);
    status = uci_set ("wireless.radio0.disabled", "0", NULL);

    status = uci_set ("wireless.default_radio0", "default_radio0", NULL);
    status = uci_set ("wireless.default_radio0.device", "radio0", NULL);
    status = uci_set ("wireless.default_radio0.network", "lan", NULL);
    status = uci_set ("wireless.default_radio0.mode", "ap", NULL);
    status = uci_set ("wireless.default_radio0.ssid", ssid, NULL);
    status = uci_set ("wireless.default_radio0.encryption", encryption, NULL);

    /* make it official */
    status = uci_commit ("wireless", NULL);
    
    /* add */
    return 0;
}
