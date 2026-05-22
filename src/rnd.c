#include "spool.h"
#include "net.h"

static void usage(void) {
    fprintf(stderr, "usage: rnd <node_root>\n");
}

static int read_size_line(int fd, size_t *size_out) {
    char line[RN_MAX_LINE];
    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) return -1;
    rn_rstrip(line);
    if (sscanf(line, "SIZE %zu", size_out) != 1) return -1;
    return 0;
}

static int read_until_end(int fd) {
    char line[RN_MAX_LINE];
    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) return -1;
    rn_rstrip(line);
    return strcmp(line, "END") == 0 ? 0 : -1;
}

static int read_payload(int fd, char **payload_out, size_t *payload_len_out) {
    size_t len = 0;
    if (read_size_line(fd, &len) != 0) return -1;
    char *buf = rn_xmalloc(len + 1);
    if (rn_net_read_exact(fd, buf, len) != 0) {
        free(buf);
        return -1;
    }
    buf[len] = '\0';
    if (read_until_end(fd) != 0) {
        free(buf);
        return -1;
    }
    *payload_out = buf;
    if (payload_len_out) *payload_len_out = len;
    return 0;
}

static int parse_simple_payload(const char *payload, char *to, size_t to_sz, char *body, size_t body_sz) {
    const char *prefix = "To: ";
    const char *header_end = strchr(payload, '\n');
    const char *sep = strstr(payload, "\n---\n");
    if (!header_end || !sep || !rn_starts_with(payload, prefix) || sep < header_end) return -1;
    size_t to_len = (size_t)(header_end - (payload + strlen(prefix)));
    if (to_len >= to_sz) to_len = to_sz - 1;
    memcpy(to, payload + strlen(prefix), to_len);
    to[to_len] = '\0';
    char *trimmed = rn_trim(to);
    if (trimmed != to) memmove(to, trimmed, strlen(trimmed) + 1);
    const char *body_start = sep + 5;
    while (*body_start == '\r' || *body_start == '\n') body_start++;
    size_t body_len = strlen(body_start);
    if (body_len >= body_sz) body_len = body_sz - 1;
    memcpy(body, body_start, body_len);
    body[body_len] = '\0';
    rn_rstrip(body);
    return 0;
}

static int now_stamp(char *out, size_t out_sz) {
    time_t now = time(NULL);
    struct tm tmv;
    if (!localtime_r(&now, &tmv)) return -1;
    return strftime(out, out_sz, "%Y%m%d%H%M%S", &tmv) == 0 ? -1 : 0;
}

static int unique_prefix_path(char *out, size_t out_sz, const char *base, const char *prefix, const char *ext) {
    char stamp[32];
    if (now_stamp(stamp, sizeof(stamp)) != 0) return -1;
    static unsigned seq = 0;
    seq++;
    return snprintf(out, out_sz, "%s/%s-%s-%u%s", base, prefix, stamp, seq, ext) >= (int)out_sz ? -1 : 0;
}

static int append_registry_log(const rn_node_config *node, const char *type, const rn_registry_event *ev) {
    char now[RN_MAX_FIELD];
    if (rn_now_iso8601(now, sizeof(now)) != 0) return -1;
    char line[RN_MAX_LINE];
    snprintf(line, sizeof(line), "%s|%s|%s|%s|%s|%s", now, type, ev->alias, ev->user_id, ev->home_node,
             ev->reason[0] ? ev->reason : "");
    return rn_config_append_registry_event(node->registry_log_path, line);
}

static int save_users_and_aliases(const rn_node_config *node, const rn_user_entry *users, size_t count) {
    if (rn_config_save_users(node->users_path, users, count) != 0) return -1;
    if (rn_config_save_aliases(node->aliases_path, users, count) != 0) return -1;
    return 0;
}

static int load_users(const rn_node_config *node, rn_user_entry **users_out, size_t *count_out) {
    if (rn_config_load_users(node->users_path, users_out, count_out) == 0) return 0;
    *users_out = NULL;
    *count_out = 0;
    return 0;
}

static int find_user_index_by_alias(const rn_user_entry *users, size_t count, const char *alias) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(users[i].alias, alias) == 0) return (int)i;
    }
    return -1;
}

static int find_user_index_by_id(const rn_user_entry *users, size_t count, const char *user_id) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(users[i].user_id, user_id) == 0) return (int)i;
    }
    return -1;
}

static void stamp_updated(rn_user_entry *u) {
    char now[RN_MAX_FIELD];
    if (rn_now_iso8601(now, sizeof(now)) == 0) rn_safe_copy(u->updated_at, sizeof(u->updated_at), now);
}

static int process_user_event(const rn_node_config *node, const char *sender_id, const rn_registry_event *ev) {
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (load_users(node, &users, &count) != 0) return -1;
    int idx = find_user_index_by_alias(users, count, ev->alias);
    int by_id = find_user_index_by_id(users, count, ev->user_id);

    if (strcmp(ev->type, "USER_REGISTER") == 0) {
        if (idx >= 0) {
            if (strcmp(users[idx].user_id, ev->user_id) == 0 && strcmp(users[idx].home_node, ev->home_node) == 0) {
                free(users);
                return 0;
            }
            if (rn_user_state_is_active(users[idx].state)) {
                rn_safe_copy(users[idx].state, sizeof(users[idx].state), "conflict");
                stamp_updated(&users[idx]);
                save_users_and_aliases(node, users, count);
                append_registry_log(node, "USER_CONFLICT", ev);
                free(users);
                return 0;
            }
            rn_safe_copy(users[idx].user_id, sizeof(users[idx].user_id), ev->user_id);
            rn_safe_copy(users[idx].home_node, sizeof(users[idx].home_node), ev->home_node);
            rn_safe_copy(users[idx].state, sizeof(users[idx].state), "pending");
            stamp_updated(&users[idx]);
            save_users_and_aliases(node, users, count);
            append_registry_log(node, "USER_REGISTER", ev);
            free(users);
            return 0;
        }
        users = rn_xrealloc(users, sizeof(*users) * (count + 1));
        idx = (int)count++;
        rn_safe_copy(users[idx].user_id, sizeof(users[idx].user_id), ev->user_id);
        rn_safe_copy(users[idx].alias, sizeof(users[idx].alias), ev->alias);
        rn_safe_copy(users[idx].home_node, sizeof(users[idx].home_node), ev->home_node);
        rn_safe_copy(users[idx].state, sizeof(users[idx].state), "pending");
        rn_now_iso8601(users[idx].created_at, sizeof(users[idx].created_at));
        rn_safe_copy(users[idx].updated_at, sizeof(users[idx].updated_at), users[idx].created_at);
        save_users_and_aliases(node, users, count);
        append_registry_log(node, "USER_REGISTER", ev);
        free(users);
        return 0;
    }

    const char *target_state = NULL;
    if (strcmp(ev->type, "USER_ACCEPT") == 0) target_state = "active";
    else if (strcmp(ev->type, "USER_REJECT") == 0) target_state = "rejected";
    else if (strcmp(ev->type, "USER_CONFLICT") == 0) target_state = "conflict";
    else if (strcmp(ev->type, "USER_REVOKE") == 0) target_state = "revoked";
    else if (strcmp(ev->type, "USER_QUARANTINE") == 0) target_state = "quarantine";
    else {
        free(users);
        return -1;
    }

    if (idx < 0 && by_id >= 0) idx = by_id;
    if (idx < 0) {
        users = rn_xrealloc(users, sizeof(*users) * (count + 1));
        idx = (int)count++;
        rn_safe_copy(users[idx].user_id, sizeof(users[idx].user_id), ev->user_id);
        rn_safe_copy(users[idx].alias, sizeof(users[idx].alias), ev->alias);
        rn_safe_copy(users[idx].home_node, sizeof(users[idx].home_node), ev->home_node);
        rn_now_iso8601(users[idx].created_at, sizeof(users[idx].created_at));
        rn_safe_copy(users[idx].updated_at, sizeof(users[idx].updated_at), users[idx].created_at);
    }
    if (ev->alias[0]) rn_safe_copy(users[idx].alias, sizeof(users[idx].alias), ev->alias);
    if (ev->user_id[0]) rn_safe_copy(users[idx].user_id, sizeof(users[idx].user_id), ev->user_id);
    if (ev->home_node[0]) rn_safe_copy(users[idx].home_node, sizeof(users[idx].home_node), ev->home_node);
    rn_safe_copy(users[idx].state, sizeof(users[idx].state), target_state);
    stamp_updated(&users[idx]);
    save_users_and_aliases(node, users, count);
    append_registry_log(node, ev->type, ev);
    free(users);
    (void)sender_id;
    return 0;
}

static int store_deadletter_payload(const rn_node_config *node, const char *payload, const char *prefix) {
    char path[RN_MAX_PATH];
    if (snprintf(path, sizeof(path), "%s/%s", node->spool_dir, "deadletter") >= (int)sizeof(path)) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    char file[RN_MAX_PATH];
    if (unique_prefix_path(file, sizeof(file), path, prefix, ".txt") != 0) return -1;
    return rn_write_entire_file(file, payload, strlen(payload));
}

static int store_registry_payload(const rn_node_config *node, const char *payload, const char *prefix) {
    char path[RN_MAX_PATH];
    if (snprintf(path, sizeof(path), "%s/%s", node->spool_dir, "registry") >= (int)sizeof(path)) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    char file[RN_MAX_PATH];
    if (unique_prefix_path(file, sizeof(file), path, prefix, ".txt") != 0) return -1;
    return rn_write_entire_file(file, payload, strlen(payload));
}

static int store_quarantine_payload(const rn_node_config *node, const char *payload, const char *prefix) {
    char path[RN_MAX_PATH];
    if (snprintf(path, sizeof(path), "%s/%s", node->spool_dir, "quarantine") >= (int)sizeof(path)) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    char file[RN_MAX_PATH];
    if (unique_prefix_path(file, sizeof(file), path, prefix, ".txt") != 0) return -1;
    return rn_write_entire_file(file, payload, strlen(payload));
}

static int load_sender_user(const rn_node_config *node, const char *sender_id, rn_user_entry *user) {
    return rn_config_find_user_record_by_id(node->users_path, sender_id, user);
}

static int sender_is_allowed(const rn_node_config *node, const char *sender_id) {
    rn_user_entry user;
    if (load_sender_user(node, sender_id, &user) == 0) return rn_user_state_is_active(user.state);
    rn_trust_entry trust;
    if (rn_config_find_trust(node->trust_path, sender_id, &trust) == 0) {
        return !rn_trust_state_is_blocked(trust.state);
    }
    return 0;
}

static int process_registry_payload(const rn_node_config *node, const char *sender_id, const char *payload) {
    rn_registry_event ev;
    if (rn_registry_event_parse(payload, &ev) != 0) return -1;
    if (!rn_registry_event_type_is_user_event(ev.type)) return -1;

    rn_trust_entry trust;
    bool trust_found = rn_config_find_trust(node->trust_path, sender_id, &trust) == 0;
    if (trust_found && rn_trust_state_is_blocked(trust.state)) {
        store_quarantine_payload(node, payload, "blocked");
        return -1;
    }
    if (trust_found && strcmp(trust.state, "quarantine") == 0) {
        store_quarantine_payload(node, payload, "trust");
        return 0;
    }
    if (store_registry_payload(node, payload, ev.type) != 0) return -1;
    return process_user_event(node, sender_id, &ev);
}

static int process_normal_message(const rn_node_config *node, const char *sender_id, const char *payload) {
    rn_message msg;
    rn_message_init(&msg);
    if (strstr(payload, "From:") && strstr(payload, "MsgID:")) {
        char *copy = rn_xstrdup(payload);
        char *sep = strstr(copy, "\n---\n");
        if (!sep) {
            free(copy);
            return -1;
        }
        *sep = '\0';
        char *headers = copy;
        char *body = sep + 5;
        char *save = NULL;
        for (char *h = strtok_r(headers, "\n", &save); h; h = strtok_r(NULL, "\n", &save)) {
            rn_rstrip(h);
            h = rn_trim(h);
            char *colon = strchr(h, ':');
            if (!colon) continue;
            *colon = '\0';
            char *key = rn_trim(h);
            char *value = rn_trim(colon + 1);
            if (strcmp(key, "To") == 0) rn_safe_copy(msg.to, sizeof(msg.to), value);
            else if (strcmp(key, "From") == 0) rn_safe_copy(msg.from, sizeof(msg.from), value);
            else if (strcmp(key, "MsgID") == 0) rn_safe_copy(msg.msg_id, sizeof(msg.msg_id), value);
            else if (strcmp(key, "Date") == 0) rn_safe_copy(msg.date, sizeof(msg.date), value);
            else if (strcmp(key, "SourceNode") == 0) rn_safe_copy(msg.source_node, sizeof(msg.source_node), value);
            else if (strcmp(key, "DestUser") == 0) rn_safe_copy(msg.dest_user, sizeof(msg.dest_user), value);
            else if (strcmp(key, "DestNode") == 0) rn_safe_copy(msg.dest_node, sizeof(msg.dest_node), value);
            else if (strcmp(key, "Status") == 0) rn_safe_copy(msg.status, sizeof(msg.status), value);
        }
        rn_safe_copy(msg.body, sizeof(msg.body), body);
        free(copy);
    } else {
        char to[RN_MAX_FIELD];
        char body[RN_MAX_BODY];
        if (parse_simple_payload(payload, to, sizeof(to), body, sizeof(body)) != 0) return -1;
        rn_safe_copy(msg.to, sizeof(msg.to), to);
        rn_safe_copy(msg.body, sizeof(msg.body), body);
    }

    rn_alias_entry sender_alias;
    bool sender_is_user = rn_route_sender_lookup(node, sender_id, &sender_alias) == 0;
    if (sender_is_user) {
        rn_user_entry sender_user;
        if (rn_config_find_user_record_by_id(node->users_path, sender_id, &sender_user) != 0 || !rn_user_state_is_active(sender_user.state)) {
            store_deadletter_payload(node, payload, "sender");
            return -1;
        }
    } else if (!sender_is_allowed(node, sender_id)) {
        store_deadletter_payload(node, payload, "blocked");
        return -1;
    }

    if (msg.from[0] == '\0') {
        if (sender_is_user) rn_safe_copy(msg.from, sizeof(msg.from), sender_alias.address);
        else rn_safe_copy(msg.from, sizeof(msg.from), sender_id);
    }
    if (msg.source_node[0] == '\0') {
        if (sender_is_user) rn_safe_copy(msg.source_node, sizeof(msg.source_node), sender_alias.node_id);
        else rn_safe_copy(msg.source_node, sizeof(msg.source_node), sender_id);
    }
    if (msg.date[0] == '\0' && rn_now_iso8601(msg.date, sizeof(msg.date)) != 0) return -1;
    if (msg.msg_id[0] == '\0' && rn_message_generate_id(msg.msg_id, sizeof(msg.msg_id)) != 0) return -1;
    if (msg.status[0] == '\0') rn_safe_copy(msg.status, sizeof(msg.status), "NEW");

    rn_route_result route;
    if (rn_route_resolve(node, msg.to, &route) != 0) {
        store_deadletter_payload(node, payload, "unknown");
        return -1;
    }
    if (!rn_user_state_is_active(route.user.state)) {
        store_deadletter_payload(node, payload, "inactive");
        return -1;
    }
    rn_safe_copy(msg.dest_user, sizeof(msg.dest_user), route.alias.user_id);
    rn_safe_copy(msg.dest_node, sizeof(msg.dest_node), route.alias.node_id);
    if (strcmp(route.alias.node_id, node->node_id) == 0) {
        if (rn_spool_store_local_message(node->root_dir, msg.dest_user, &msg, NULL, 0) != 0) {
            store_deadletter_payload(node, payload, "store");
            return -1;
        }
    } else {
        if (rn_spool_store_peer_message(node->root_dir, msg.dest_node, &msg, NULL, 0) != 0) {
            store_deadletter_payload(node, payload, "peer");
            return -1;
        }
    }
    return 0;
}

static int handle_push(int fd, const rn_node_config *node, const char *sender_id) {
    char *payload = NULL;
    size_t payload_len = 0;
    if (read_payload(fd, &payload, &payload_len) != 0) {
        rn_net_write_line(fd, "ERR BAD_PAYLOAD");
        return -1;
    }

    int rc = 0;
    if (strstr(payload, "Type: ") == payload) {
        rc = process_registry_payload(node, sender_id, payload);
    } else {
        rc = process_normal_message(node, sender_id, payload);
    }
    free(payload);
    if (rc != 0) {
        rn_net_write_line(fd, "ERR ROUTE");
        return -1;
    }
    char reply[RN_MAX_LINE];
    snprintf(reply, sizeof(reply), "OK %lu", (unsigned long)time(NULL));
    rn_net_write_line(fd, reply);
    return 0;
}

static int handle_pull(int fd, const rn_node_config *node, const char *user_id) {
    char inbox[RN_MAX_PATH];
    if (snprintf(inbox, sizeof(inbox), "%s/users/%s/inbox", node->spool_dir, user_id) >= (int)sizeof(inbox)) return -1;
    char **paths = NULL;
    size_t count = 0;
    if (rn_spool_load_message_paths(inbox, &paths, &count) != 0) count = 0;
    char reply[RN_MAX_LINE];
    snprintf(reply, sizeof(reply), "COUNT %zu", count);
    if (rn_net_write_line(fd, reply) != 0) return -1;
    for (size_t i = 0; i < count; i++) {
        rn_message msg;
        if (rn_message_read_file(paths[i], &msg) != 0) continue;
        char *serialized = NULL;
        size_t len = 0;
        if (rn_message_serialize(&msg, &serialized, &len) != 0) continue;
        snprintf(reply, sizeof(reply), "SIZE %zu", len);
        if (rn_net_write_line(fd, reply) != 0) {
            free(serialized);
            break;
        }
        if (rn_net_write_all(fd, serialized, len) != 0 || rn_net_write_line(fd, "END") != 0) {
            free(serialized);
            break;
        }
        free(serialized);
    }
    if (rn_net_write_line(fd, "DONE") != 0) {
        for (size_t i = 0; i < count; i++) free(paths[i]);
        free(paths);
        return -1;
    }
    for (size_t i = 0; i < count; i++) free(paths[i]);
    free(paths);
    return 0;
}

static int handle_ack(int fd, const char *msgid) {
    (void)msgid;
    return rn_net_write_line(fd, "OK");
}

static int handle_client(int fd, const rn_node_config *node) {
    char line[RN_MAX_LINE];
    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) return -1;
    rn_rstrip(line);
    if (!rn_starts_with(line, "HELLO ")) return -1;
    char sender_id[RN_MAX_FIELD];
    rn_safe_copy(sender_id, sizeof(sender_id), rn_trim(line + 6));

    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) return -1;
    rn_rstrip(line);
    if (strcmp(line, "PUSH") == 0) return handle_push(fd, node, sender_id);
    if (rn_starts_with(line, "PULL ")) return handle_pull(fd, node, rn_trim(line + 5));
    if (rn_starts_with(line, "ACK ")) return handle_ack(fd, rn_trim(line + 4));
    return -1;
}

int main(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    if (argc < 2) {
        usage();
        return 1;
    }
    const char *node_root = argv[1];
    rn_node_config node;
    if (rn_config_load_node(node_root, &node) != 0) {
        fprintf(stderr, "rnd: failed to load node config from %s\n", node_root);
        return 1;
    }
    if (rn_spool_prepare_node(node_root) != 0) {
        fprintf(stderr, "rnd: failed to prepare spool\n");
        return 1;
    }
    int listen_fd = rn_net_listen(node.host, node.port);
    if (listen_fd < 0) {
        perror("rnd listen");
        return 1;
    }
    fprintf(stderr, "rnd listening on %s:%d (%s)\n", node.host, node.port, node.node_id);
    for (;;) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd < 0) continue;
        handle_client(client_fd, &node);
        rn_net_close(client_fd);
    }
}
