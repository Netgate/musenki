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

int wifi_transaction_begin(clicon_handle h, transaction_data td)
{
	clicon_log(LOG_NOTICE, "%s called", __func__);

	return 0;
}
