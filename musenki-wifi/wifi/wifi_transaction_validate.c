/* -*- c-basic-offset: 8; -*- */
#pragma GCC optimize("O0")
/*
 * Copyright 2021 Rubicon Communications LLC
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>

#include <cligen/cligen.h>
#include <clixon/clixon.h>
#include <clixon/clixon_backend.h>
#include <clixon/clixon_xml.h>
#include <clixon/clixon_xpath.h>

#include "wifi.h"

int wifi_transaction_validate(clicon_handle h, transaction_data td)
{
	clicon_log(LOG_NOTICE, "%s called", __func__);

	/* assume only one radio named radio0 */
	
	/* fetch wireless status "ubus call network.wireless status" */
	/* parse json - to XML? */
	
	/* if country is not set and we're not setting it then error */

	/* if hwmode is not set and we're not setting it then error */

	/* if htmode is not set and we're not setting it then error */

	/* check channel setting against hwmode */

	/* check channel setting against country */

	return 0;
}
