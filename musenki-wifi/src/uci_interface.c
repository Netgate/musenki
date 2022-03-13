/* -*- c-basic-offset: 4; -*- */

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

#include "do_command.h"

#define UCI_CMD "/sbin/uci"

/*
 * for right now this calls the cli command 'uci' - there is a not-well
 * documented C API that we should use.
 *
 * config - <config>
 * option - <config>.<section>.<option>
 * path - <config>[.<section>[[.<option>]]]
 */

int uci_add  (char *config, char *section_type, cbuf *result)
{
    /* uci add <config> <section-type> */
    char *argv[] = { "uci", "add", section_type, NULL };

    return do_command (UCI_CMD, argv, 10, result);
}

int uci_add_list (char *option, char *value, cbuf *result)
{
    /* uci add_list <config>.<section>.<option>=<string> */
    char *argv[] = { "uci", "add_list", value, NULL };
	
    return do_command (UCI_CMD, argv, 10, result);
}

int uci_changes (cbuf *result)
{
    /* uci changes */
    char *argv[] = { "uci", "changes", NULL };

    return do_command (UCI_CMD, argv, 10, result);
}

int uci_commit (char *config, cbuf *result)
{
    /* uci commit [<config>] */
    char *argv[] = { "uci", "commit", config, NULL };

    return do_command (UCI_CMD, argv, 10, result);
}

int uci_del_list (char *option, char *value, cbuf *result)
{
    /* uci del_list <config>.<section>.<option>=<string> */
    char *argv[] = { "uci", "add", option, value, NULL };
	
    return do_command (UCI_CMD, argv, 10, result);
}

int uci_delete (char *path, char *id, cbuf *result)
{
	/* uci delete <config>[.<section>[[.<option>][=<id>]]] */

	char *argv[4];
	char buf[128];

	if (id == NULL) {
	    argv[0] = "uci";
	    argv[1] = "delete";
	    argv[2] = path;
	    argv[3] = NULL;
	}
	else {
	    sprintf (buf, "%s=%s", path, id);
	    argv[0] = "uci";
	    argv[1] = "delete";
	    argv[2] = buf;
	    argv[3] = NULL;
	}

	return do_command(UCI_CMD, argv, 10, result);
}

int uci_get (char *path, cbuf *result)
{
    /* uci get <config>.<section>[.<option>] */
    char *argv[] = { "uci", "get", path, NULL };
	
    return do_command (UCI_CMD, argv, 10, result);
}

int uci_rename (char *path, char *name, cbuf *result)
{
	/* uci rename <config>.<section>[.<option>]=<name> */
    char *argv[] = { "uci", "rename", path, name, NULL };
	
	return do_command (UCI_CMD, argv, 10, result);
}


int uci_revert (char *path, cbuf *result)
{
	/* uci revert <config>[.<section>[.<option>]] */
    char *argv[] = { "uci", "revert", path, NULL };

	return do_command (UCI_CMD, argv, 10, result);
}

int uci_set (char *path, char *value, cbuf *result)
{
    /* uci set <config>.<section>[.<option>]=<value> */
    char buf[128];

    sprintf (buf, "%s=%s", path, value);
    char *argv[] = { "uci", "set", buf, NULL };
	
    return do_command (UCI_CMD, argv, 10, result);
}

int uci_show (char *path, cbuf *result)
{
    /* uci show [<config>[.<section>[.<option>]]] */
    char *argv[] = { "uci", "show", path, NULL };

    return do_command (UCI_CMD, argv, 10, result);
}



