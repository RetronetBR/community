#ifndef RETRONET_ROUTING_H
#define RETRONET_ROUTING_H

#include "config.h"
#include "message.h"

typedef struct {
    rn_alias_entry alias;
    rn_user_entry user;
    rn_peer_entry peer;
    bool is_local;
} rn_route_result;

int rn_route_resolve(const rn_node_config *node, const char *address, rn_route_result *out);
int rn_route_sender_lookup(const rn_node_config *node, const char *user_id, rn_alias_entry *out);

#endif
