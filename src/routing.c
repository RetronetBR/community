#include "routing.h"

int rn_route_sender_lookup(const rn_node_config *node, const char *user_id, rn_alias_entry *out) {
    return rn_config_find_user_by_id(node->aliases_path, user_id, out);
}

int rn_route_resolve(const rn_node_config *node, const char *address, rn_route_result *out) {
    memset(out, 0, sizeof(*out));
    rn_alias_entry alias;
    if (rn_config_find_alias(node->aliases_path, address, &alias) != 0) return -1;
    out->alias = alias;
    if (rn_config_find_user_record_by_alias(node->users_path, alias.address, &out->user) != 0) return -1;
    out->is_local = strcmp(alias.node_id, node->node_id) == 0;
    if (!out->is_local) {
        if (rn_config_find_peer(node->peers_path, alias.node_id, &out->peer) != 0) return -1;
    }
    return 0;
}
