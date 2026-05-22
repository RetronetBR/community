#ifndef RETRONET_SPOOL_H
#define RETRONET_SPOOL_H

#include "routing.h"

int rn_spool_prepare_node(const char *node_root);
int rn_spool_store_local_message(const char *node_root, const char *user_id, const rn_message *msg, char *out_path, size_t out_sz);
int rn_spool_store_peer_message(const char *node_root, const char *peer_node_id, const rn_message *msg, char *out_path, size_t out_sz);
int rn_spool_store_client_message(const char *client_root, const rn_message *msg, char *out_path, size_t out_sz);
int rn_spool_store_deadletter_text(const char *node_root, const char *name, const char *text);
int rn_spool_store_registry_text(const char *node_root, const char *name, const char *text);
int rn_spool_store_quarantine_text(const char *node_root, const char *name, const char *text);
int rn_spool_list_directory(const char *dir, char ***paths_out, size_t *count_out);
int rn_spool_load_message_paths(const char *dir, char ***paths_out, size_t *count_out);
int rn_spool_move_file(const char *src, const char *dst);

#endif
