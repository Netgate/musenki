/* -*- c-basic-offset: 8; -*- */
#pragma GCC optimize("O0")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cligen/cligen.h>
#include <clixon/clixon.h>
#include <clixon/clixon_backend.h>
#include <clixon/clixon_xml.h>
#include <clixon/clixon_xpath.h>

#include "do_command.h"

/*
 * this code controls wifi status (up, down, etc)
 * it does not handle wifi configuration
 */

/*
 * use cli wifi command - is C API available?
 */

/*
 * Usage: /sbin/wifi [config|up|down|reconf|reload|status]
 * enables (default), disables or configures devices not yet configured.
 */

int wifi_config(void)
{
	char *argv[] = { "config", NULL };

	return do_command("wifi", argv, 10, NULL);
}

int wifi_up(void)
{
	char *argv[] = { "up", NULL };

	return do_command("wifi", argv, 10, NULL);
}

int wifi_down(void)
{
	char *argv[] = { "down", NULL };

	return do_command("wifi", argv, 10, NULL);
}

int wifi_reconf(void)
{
	char *argv[] = { "reconf", NULL };

	return do_command("wifi", argv, 10, NULL);
}

int wifi_reload(void)
{
	char *argv[] = { "reload", NULL };

	return do_command("wifi", argv, 10, NULL);
}

int wifi_status(cbuf *status)
{
	char *argv[] = { "status", NULL };

	return do_command("wifi", argv, 10, status);
}
