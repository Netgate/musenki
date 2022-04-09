/* -*- c-basic-offset: 4; -*- */
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

static clixon_plugin_api api =
{
     .ca_name = "musenki wifi backend",
     .ca_init = clixon_plugin_init,

     /* all transaction stages */
     .ca_trans_begin = wifi_transaction_begin,
     .ca_trans_validate = wifi_transaction_validate,
     .ca_trans_complete = NULL,
     .ca_trans_commit = wifi_transaction_commit,
     .ca_trans_commit_done = NULL,
     .ca_trans_revert = NULL,
     .ca_trans_end = wifi_transaction_end,
     .ca_trans_abort = NULL
};

clixon_plugin_api *clixon_plugin_init(clicon_handle h)
{
    clicon_log(LOG_NOTICE, "wifi initializing");
	
    return &api;
}
