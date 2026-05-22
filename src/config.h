#ifndef RETRONET_CONFIG_H
#define RETRONET_CONFIG_H

#include "common.h"

typedef struct {
    char node_id[RN_MAX_FIELD];
    char node_alias[RN_MAX_FIELD];
    char host[RN_MAX_FIELD];
    int port;
    char caps[RN_MAX_LINE];
    char root_dir[RN_MAX_PATH];
    char config_dir[RN_MAX_PATH];
    char spool_dir[RN_MAX_PATH];
    char aliases_path[RN_MAX_PATH];
    char peers_path[RN_MAX_PATH];
    char users_path[RN_MAX_PATH];
    char trust_path[RN_MAX_PATH];
    char registry_log_path[RN_MAX_PATH];
} rn_node_config;

typedef struct {
    char user_id[RN_MAX_FIELD];
    char address[RN_MAX_FIELD];
    char node_id[RN_MAX_FIELD];
    char node_root[RN_MAX_PATH];
    char default_node[RN_MAX_FIELD];
    char default_host[RN_MAX_FIELD];
    int default_port;
    char drafts_dir[RN_MAX_PATH];
    int auto_sync;
    int auto_push;
    int auto_pull;
    char token[RN_MAX_FIELD];
    char root_dir[RN_MAX_PATH];
    char config_dir[RN_MAX_PATH];
    char inbox_dir[RN_MAX_PATH];
    char outbox_dir[RN_MAX_PATH];
} rn_client_config;

typedef struct {
    char address[RN_MAX_FIELD];
    char user_id[RN_MAX_FIELD];
    char node_id[RN_MAX_FIELD];
} rn_alias_entry;

typedef struct {
    char user_id[RN_MAX_FIELD];
    char alias[RN_MAX_FIELD];
    char home_node[RN_MAX_FIELD];
    char state[RN_MAX_FIELD];
    char created_at[RN_MAX_FIELD];
    char updated_at[RN_MAX_FIELD];
} rn_user_entry;

typedef struct {
    char node_id[RN_MAX_FIELD];
    char node_alias[RN_MAX_FIELD];
    char state[RN_MAX_FIELD];
    int reg_limit_per_day;
} rn_trust_entry;

typedef struct {
    char node_id[RN_MAX_FIELD];
    char node_alias[RN_MAX_FIELD];
    char proto[RN_MAX_FIELD];
    char host[RN_MAX_FIELD];
    int port;
    char status[RN_MAX_FIELD];
} rn_peer_entry;

int rn_config_load_node(const char *root_dir, rn_node_config *cfg);
int rn_config_load_client(const char *root_dir, rn_client_config *cfg);
int rn_config_load_aliases(const char *path, rn_alias_entry **out, size_t *count);
int rn_config_load_users(const char *path, rn_user_entry **out, size_t *count);
int rn_config_load_trust(const char *path, rn_trust_entry **out, size_t *count);
int rn_config_load_peers(const char *path, rn_peer_entry **out, size_t *count);
int rn_config_find_alias(const char *aliases_path, const char *address, rn_alias_entry *out);
int rn_config_find_user_by_id(const char *aliases_path, const char *user_id, rn_alias_entry *out);
int rn_config_find_user_record(const char *users_path, const char *alias_or_user_id, rn_user_entry *out);
int rn_config_find_user_record_by_alias(const char *users_path, const char *alias, rn_user_entry *out);
int rn_config_find_user_record_by_id(const char *users_path, const char *user_id, rn_user_entry *out);
int rn_config_save_users(const char *path, const rn_user_entry *entries, size_t count);
int rn_config_save_aliases(const char *path, const rn_user_entry *entries, size_t count);
int rn_config_append_registry_event(const char *path, const char *line);
int rn_config_append_registry_payload(const char *dir, const char *event_id, const char *payload, size_t len);
int rn_config_next_user_id(const char *users_path, const char *home_node, char *out, size_t out_sz);
int rn_config_find_trust(const char *trust_path, const char *node_id, rn_trust_entry *out);
int rn_config_find_peer(const char *peers_path, const char *node_id, rn_peer_entry *out);

bool rn_user_state_is_active(const char *state);
bool rn_user_state_is_terminal(const char *state);
bool rn_trust_state_is_blocked(const char *state);

#endif
