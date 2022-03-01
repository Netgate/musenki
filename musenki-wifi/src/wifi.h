/*
 * Copyright 2021, Rubicon Communications LLC
 */

int wifi_state_data(clicon_handle h, cvec *nsc, char *xpath, cxobj *xtop);

int wifi_transaction_begin(clicon_handle h, transaction_data td);
int wifi_transaction_validate(clicon_handle h, transaction_data td);
int wifi_transaction_complete(clicon_handle h, transaction_data td);
int wifi_transaction_commit(clicon_handle h, transaction_data td);
int wifi_transaction_commit_done(clicon_handle h, transaction_data td);
int wifi_transaction_revert(clicon_handle h, transaction_data td);
int wifi_transaction_end(clicon_handle h, transaction_data td);
int wifi_transaction_abort(clicon_handle h, transaction_data td);
