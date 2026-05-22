#include "spool.h"

static void usage(void) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  rndctl user request <alias> --node-root <node_root>\n");
    fprintf(stderr, "  rndctl user approve <alias> --node-root <node_root>\n");
    fprintf(stderr, "  rndctl user reject <alias> --node-root <node_root>\n");
    fprintf(stderr, "  rndctl user conflict <alias> --node-root <node_root>\n");
    fprintf(stderr, "  rndctl user status <alias> --node-root <node_root>\n");
    fprintf(stderr, "  rndctl user list --node-root <node_root>\n");
    fprintf(stderr, "  rndctl aliases rebuild --node-root <node_root>\n");
}

static int parse_node_root(int argc, char **argv, const char **node_root) {
    for (int i = 0; i < argc - 1; i++) {
        if (strcmp(argv[i], "--node-root") == 0) {
            *node_root = argv[i + 1];
            return 0;
        }
    }
    return -1;
}

static int load_node(const char *root, rn_node_config *node) {
    if (rn_config_load_node(root, node) != 0) return -1;
    if (rn_spool_prepare_node(root) != 0) return -1;
    return 0;
}

static void event_clear(rn_registry_event *ev) {
    rn_registry_event_init(ev);
}

static int write_event_payload(const rn_node_config *node, const rn_registry_event *ev) {
    char *payload = NULL;
    size_t len = 0;
    if (rn_registry_event_serialize(ev, &payload, &len) != 0) return -1;
    char name[RN_MAX_PATH];
    char stamp[32];
    time_t now = time(NULL);
    struct tm tmv;
    if (!localtime_r(&now, &tmv) || strftime(stamp, sizeof(stamp), "%Y%m%d%H%M%S", &tmv) == 0) {
        free(payload);
        return -1;
    }
    if (snprintf(name, sizeof(name), "%s-%s-%s.txt", ev->type, stamp, ev->user_id) >= (int)sizeof(name)) {
        free(payload);
        return -1;
    }
    int rc = rn_spool_store_registry_text(node->root_dir, name, payload);
    free(payload);
    return rc;
}

static int log_event(const rn_node_config *node, const char *type, const rn_user_entry *u, const char *details) {
    char now[RN_MAX_FIELD];
    if (rn_now_iso8601(now, sizeof(now)) != 0) return -1;
    char line[RN_MAX_LINE];
    snprintf(line, sizeof(line), "%s|%s|%s|%s|%s|%s", now, type, u->alias, u->user_id, u->home_node,
             details ? details : "");
    return rn_config_append_registry_event(node->registry_log_path, line);
}

static int save_users(const rn_node_config *node, rn_user_entry *users, size_t count) {
    if (rn_config_save_users(node->users_path, users, count) != 0) return -1;
    if (rn_config_save_aliases(node->aliases_path, users, count) != 0) return -1;
    return 0;
}

static void stamp_updated(rn_user_entry *user) {
    char now[RN_MAX_FIELD];
    if (rn_now_iso8601(now, sizeof(now)) == 0) rn_safe_copy(user->updated_at, sizeof(user->updated_at), now);
}

static int count_registers_today(const rn_node_config *node) {
    FILE *f = fopen(node->registry_log_path, "r");
    if (!f) return 0;
    char today[16];
    time_t now = time(NULL);
    struct tm tmv;
    if (!localtime_r(&now, &tmv) || strftime(today, sizeof(today), "%Y-%m-%d", &tmv) == 0) {
        fclose(f);
        return 0;
    }
    char line[RN_MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, today, strlen(today)) != 0) continue;
        if (strstr(line, "|USER_REGISTER|") != NULL) count++;
    }
    fclose(f);
    return count;
}

static int find_idx_by_alias(const rn_user_entry *users, size_t count, const char *alias) {
    for (size_t i = 0; i < count; i++) if (strcmp(users[i].alias, alias) == 0) return (int)i;
    return -1;
}

static int request_user(const rn_node_config *node, const char *alias) {
    rn_trust_entry trust;
    if (rn_config_find_trust(node->trust_path, node->node_id, &trust) == 0) {
        int reg_count = count_registers_today(node);
        if (reg_count >= trust.reg_limit_per_day) {
            fprintf(stderr, "registration limit exceeded\n");
            return 1;
        }
        if (strcmp(trust.state, "blocked") == 0) {
            fprintf(stderr, "node blocked\n");
            return 1;
        }
    }
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (rn_config_load_users(node->users_path, &users, &count) != 0) return -1;
    int existing = find_idx_by_alias(users, count, alias);
    if (existing >= 0) {
        rn_registry_event ev;
        event_clear(&ev);
        rn_safe_copy(ev.type, sizeof(ev.type), "USER_REJECT");
        rn_safe_copy(ev.alias, sizeof(ev.alias), users[existing].alias);
        rn_safe_copy(ev.user_id, sizeof(ev.user_id), users[existing].user_id);
        rn_safe_copy(ev.home_node, sizeof(ev.home_node), users[existing].home_node);
        rn_safe_copy(ev.reason, sizeof(ev.reason), "alias already exists");
        write_event_payload(node, &ev);
        log_event(node, "USER_REJECT", &users[existing], "alias already exists");
        free(users);
        fprintf(stderr, "alias already exists\n");
        return 1;
    }
    users = rn_xrealloc(users, sizeof(*users) * (count + 1));
    rn_user_entry *u = &users[count++];
    rn_safe_copy(u->alias, sizeof(u->alias), alias);
    rn_safe_copy(u->home_node, sizeof(u->home_node), node->node_id);
    rn_safe_copy(u->state, sizeof(u->state), "pending");
    if (rn_config_next_user_id(node->users_path, node->node_id, u->user_id, sizeof(u->user_id)) != 0) {
        free(users);
        return -1;
    }
    rn_now_iso8601(u->created_at, sizeof(u->created_at));
    rn_safe_copy(u->updated_at, sizeof(u->updated_at), u->created_at);
    if (save_users(node, users, count) != 0) {
        free(users);
        return -1;
    }
    rn_registry_event ev;
    event_clear(&ev);
    rn_safe_copy(ev.type, sizeof(ev.type), "USER_REGISTER");
    rn_safe_copy(ev.alias, sizeof(ev.alias), u->alias);
    rn_safe_copy(ev.user_id, sizeof(ev.user_id), u->user_id);
    rn_safe_copy(ev.home_node, sizeof(ev.home_node), u->home_node);
    rn_safe_copy(ev.created_by, sizeof(ev.created_by), node->node_alias);
    write_event_payload(node, &ev);
    log_event(node, "USER_REGISTER", u, "pending");
    free(users);
    return 0;
}

static int change_user_state(const rn_node_config *node, const char *alias, const char *state, const char *event_type) {
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (rn_config_load_users(node->users_path, &users, &count) != 0) return -1;
    int idx = find_idx_by_alias(users, count, alias);
    if (idx < 0) {
        free(users);
        fprintf(stderr, "alias not found\n");
        return 1;
    }
    rn_safe_copy(users[idx].state, sizeof(users[idx].state), state);
    stamp_updated(&users[idx]);
    if (save_users(node, users, count) != 0) {
        free(users);
        return -1;
    }
    rn_registry_event ev;
    event_clear(&ev);
    rn_safe_copy(ev.type, sizeof(ev.type), event_type);
    rn_safe_copy(ev.alias, sizeof(ev.alias), users[idx].alias);
    rn_safe_copy(ev.user_id, sizeof(ev.user_id), users[idx].user_id);
    rn_safe_copy(ev.home_node, sizeof(ev.home_node), users[idx].home_node);
    write_event_payload(node, &ev);
    log_event(node, event_type, &users[idx], state);
    free(users);
    return 0;
}

static int list_users(const rn_node_config *node) {
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (rn_config_load_users(node->users_path, &users, &count) != 0) return -1;
    for (size_t i = 0; i < count; i++) {
        printf("%s|%s|%s|%s|%s|%s\n", users[i].user_id, users[i].alias, users[i].home_node, users[i].state,
               users[i].created_at, users[i].updated_at);
    }
    free(users);
    return 0;
}

static int status_user(const rn_node_config *node, const char *alias) {
    rn_user_entry user;
    if (rn_config_find_user_record_by_alias(node->users_path, alias, &user) != 0) return 1;
    printf("USER_ID=%s\nALIAS=%s\nHOME_NODE=%s\nSTATE=%s\nCREATED_AT=%s\nUPDATED_AT=%s\n",
           user.user_id, user.alias, user.home_node, user.state, user.created_at, user.updated_at);
    return 0;
}

static int rebuild_aliases(const rn_node_config *node) {
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (rn_config_load_users(node->users_path, &users, &count) != 0) return -1;
    int rc = rn_config_save_aliases(node->aliases_path, users, count);
    free(users);
    return rc;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "user") == 0) {
        const char *action = argv[2];
        const char *node_root = NULL;
        if (parse_node_root(argc - 2, &argv[2], &node_root) != 0) {
            usage();
            return 1;
        }
        rn_node_config node;
        if (load_node(node_root, &node) != 0) return 1;
        if (strcmp(action, "list") == 0) return list_users(&node) == 0 ? 0 : 1;
        if (strcmp(action, "request") == 0 && argc >= 5) return request_user(&node, argv[3]) == 0 ? 0 : 1;
        if (strcmp(action, "status") == 0 && argc >= 5) return status_user(&node, argv[3]) == 0 ? 0 : 1;
        if (strcmp(action, "approve") == 0 && argc >= 5) return change_user_state(&node, argv[3], "active", "USER_ACCEPT") == 0 ? 0 : 1;
        if (strcmp(action, "reject") == 0 && argc >= 5) return change_user_state(&node, argv[3], "rejected", "USER_REJECT") == 0 ? 0 : 1;
        if (strcmp(action, "conflict") == 0 && argc >= 5) return change_user_state(&node, argv[3], "conflict", "USER_CONFLICT") == 0 ? 0 : 1;
        if (strcmp(action, "revoke") == 0 && argc >= 5) return change_user_state(&node, argv[3], "revoked", "USER_REVOKE") == 0 ? 0 : 1;
        if (strcmp(action, "quarantine") == 0 && argc >= 5) return change_user_state(&node, argv[3], "quarantine", "USER_QUARANTINE") == 0 ? 0 : 1;
        usage();
        return 1;
    }

    if (strcmp(argv[1], "aliases") == 0 && strcmp(argv[2], "rebuild") == 0) {
        const char *node_root = NULL;
        if (parse_node_root(argc - 3, &argv[3], &node_root) != 0) {
            usage();
            return 1;
        }
        rn_node_config node;
        if (load_node(node_root, &node) != 0) return 1;
        return rebuild_aliases(&node) == 0 ? 0 : 1;
    }

    usage();
    return 1;
}
