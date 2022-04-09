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

#include "system.h"

static clixon_plugin_api api =
{
     .ca_name = "musenki system backend",
     .ca_init = clixon_plugin_init,

};

clixon_plugin_api *clixon_plugin_init(clicon_handle h)
{
    clicon_log(LOG_NOTICE, "system initializing");
	
    return &api;
}
