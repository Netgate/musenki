/* -*- c-basic-offset: 4; -*- */

int uci_add (char *config, char *section_type, cbuf *result);
int uci_add_list (char *option, char *value, cbuf *result);
int uci_commit (char *config, cbuf *result);
int uci_del_list (char *option, char *value, cbuf *result);
int uci_delete (char *path, char *id, cbuf *result);
int uci_get (char *path, cbuf *result);
int uci_rename (char *path, char *name, cbuf *result);
int uci_revert (char *path, cbuf *result);
int uci_set (char *path, char *value, cbuf *result);
int uci_show (char *path, cbuf *result);
