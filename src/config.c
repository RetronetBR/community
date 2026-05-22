#include "config.h"

static int parse_key_value_file(const char *path, void (*cb)(const char *, const char *, void *), void *user) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char line[RN_MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        rn_rstrip(line);
        char *trim = rn_trim(line);
        if (*trim == '\0' || *trim == '#') continue;
        char *eq = strchr(trim, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = rn_trim(trim);
        char *value = rn_trim(eq + 1);
        cb(key, value, user);
    }
    fclose(f);
    return 0;
}

struct node_loader_ctx {
    rn_node_config *cfg;
    const char *root_dir;
};

static void node_loader_cb(const char *key, const char *value, void *user) {
    struct node_loader_ctx *ctx = user;
    rn_node_config *cfg = ctx->cfg;
    if (strcmp(key, "NODE_ID") == 0) rn_safe_copy(cfg->node_id, sizeof(cfg->node_id), value);
    else if (strcmp(key, "NODE_ALIAS") == 0) rn_safe_copy(cfg->node_alias, sizeof(cfg->node_alias), value);
    else if (strcmp(key, "HOST") == 0) rn_safe_copy(cfg->host, sizeof(cfg->host), value);
    else if (strcmp(key, "PORT") == 0) cfg->port = atoi(value);
    else if (strcmp(key, "CAPS") == 0) rn_safe_copy(cfg->caps, sizeof(cfg->caps), value);
}

struct client_loader_ctx {
    rn_client_config *cfg;
    const char *root_dir;
};

static void client_loader_cb(const char *key, const char *value, void *user) {
    struct client_loader_ctx *ctx = user;
    rn_client_config *cfg = ctx->cfg;
    if (strcmp(key, "USER_ID") == 0) rn_safe_copy(cfg->user_id, sizeof(cfg->user_id), value);
    else if (strcmp(key, "ADDRESS") == 0) rn_safe_copy(cfg->address, sizeof(cfg->address), value);
    else if (strcmp(key, "NODE_ID") == 0) rn_safe_copy(cfg->node_id, sizeof(cfg->node_id), value);
    else if (strcmp(key, "NODE_ROOT") == 0) rn_safe_copy(cfg->node_root, sizeof(cfg->node_root), value);
    else if (strcmp(key, "DEFAULT_NODE") == 0) rn_safe_copy(cfg->default_node, sizeof(cfg->default_node), value);
    else if (strcmp(key, "DEFAULT_HOST") == 0) rn_safe_copy(cfg->default_host, sizeof(cfg->default_host), value);
    else if (strcmp(key, "DEFAULT_PORT") == 0) cfg->default_port = atoi(value);
    else if (strcmp(key, "DRAFTS_DIR") == 0) rn_safe_copy(cfg->drafts_dir, sizeof(cfg->drafts_dir), value);
    else if (strcmp(key, "AUTO_SYNC") == 0) cfg->auto_sync = atoi(value);
    else if (strcmp(key, "AUTO_PUSH") == 0) cfg->auto_push = atoi(value);
    else if (strcmp(key, "AUTO_PULL") == 0) cfg->auto_pull = atoi(value);
    else if (strcmp(key, "TOKEN") == 0) rn_safe_copy(cfg->token, sizeof(cfg->token), value);
}

int rn_config_load_node(const char *root_dir, rn_node_config *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    rn_safe_copy(cfg->root_dir, sizeof(cfg->root_dir), root_dir);
    if (rn_join3_path(cfg->config_dir, sizeof(cfg->config_dir), root_dir, "config", "node.conf") != 0) return -1;
    if (rn_join3_path(cfg->spool_dir, sizeof(cfg->spool_dir), root_dir, "spool", "") != 0) return -1;
    size_t n = strlen(cfg->spool_dir);
    if (n > 0 && cfg->spool_dir[n - 1] == '/') cfg->spool_dir[n - 1] = '\0';
    if (rn_join3_path(cfg->aliases_path, sizeof(cfg->aliases_path), root_dir, "config", "aliases.db") != 0) return -1;
    if (rn_join3_path(cfg->peers_path, sizeof(cfg->peers_path), root_dir, "config", "peers.db") != 0) return -1;
    if (rn_join3_path(cfg->users_path, sizeof(cfg->users_path), root_dir, "config", "users.db") != 0) return -1;
    if (rn_join3_path(cfg->trust_path, sizeof(cfg->trust_path), root_dir, "config", "trust.db") != 0) return -1;
    if (rn_join3_path(cfg->registry_log_path, sizeof(cfg->registry_log_path), root_dir, "config", "registry_events.log") != 0) return -1;
    struct node_loader_ctx ctx = {.cfg = cfg, .root_dir = root_dir};
    return parse_key_value_file(cfg->config_dir, node_loader_cb, &ctx);
}

int rn_config_load_client(const char *root_dir, rn_client_config *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    rn_safe_copy(cfg->root_dir, sizeof(cfg->root_dir), root_dir);
    if (rn_join3_path(cfg->config_dir, sizeof(cfg->config_dir), root_dir, "config", "client.conf") != 0) return -1;
    if (rn_join_path(cfg->inbox_dir, sizeof(cfg->inbox_dir), root_dir, "inbox") != 0) return -1;
    if (rn_join_path(cfg->outbox_dir, sizeof(cfg->outbox_dir), root_dir, "outbox") != 0) return -1;
    if (rn_join_path(cfg->drafts_dir, sizeof(cfg->drafts_dir), root_dir, "drafts") != 0) return -1;
    struct client_loader_ctx ctx = {.cfg = cfg, .root_dir = root_dir};
    if (parse_key_value_file(cfg->config_dir, client_loader_cb, &ctx) != 0) return -1;
    if (cfg->node_root[0] != '\0' && !rn_path_is_abs(cfg->node_root)) {
        char resolved[RN_MAX_PATH];
        if (rn_resolve_path(resolved, sizeof(resolved), root_dir, cfg->node_root) != 0) return -1;
        rn_safe_copy(cfg->node_root, sizeof(cfg->node_root), resolved);
    }
    if (cfg->drafts_dir[0] != '\0' && !rn_path_is_abs(cfg->drafts_dir)) {
        char resolved[RN_MAX_PATH];
        if (rn_resolve_path(resolved, sizeof(resolved), root_dir, cfg->drafts_dir) != 0) return -1;
        rn_safe_copy(cfg->drafts_dir, sizeof(cfg->drafts_dir), resolved);
    }
    if (cfg->default_host[0] == '\0') rn_safe_copy(cfg->default_host, sizeof(cfg->default_host), "127.0.0.1");
    if (cfg->default_port == 0) cfg->default_port = 2323;
    return 0;
}

static int load_lines(const char *path, char ***lines_out, size_t *count_out) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char **lines = NULL;
    size_t count = 0;
    char line[RN_MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        rn_rstrip(line);
        char *trim = rn_trim(line);
        if (*trim == '\0' || *trim == '#') continue;
        lines = rn_xrealloc(lines, sizeof(char *) * (count + 1));
        lines[count++] = rn_xstrdup(trim);
    }
    fclose(f);
    *lines_out = lines;
    *count_out = count;
    return 0;
}

int rn_config_load_aliases(const char *path, rn_alias_entry **out, size_t *count) {
    *out = NULL;
    *count = 0;
    char **lines = NULL;
    size_t n = 0;
    if (load_lines(path, &lines, &n) != 0) return -1;
    rn_alias_entry *entries = NULL;
    for (size_t i = 0; i < n; i++) {
        char *save = NULL;
        char *addr = strtok_r(lines[i], "|", &save);
        char *user = strtok_r(NULL, "|", &save);
        char *node = strtok_r(NULL, "|", &save);
        if (!addr || !user || !node) continue;
        entries = rn_xrealloc(entries, sizeof(*entries) * (*count + 1));
        rn_safe_copy(entries[*count].address, sizeof(entries[*count].address), rn_trim(addr));
        rn_safe_copy(entries[*count].user_id, sizeof(entries[*count].user_id), rn_trim(user));
        rn_safe_copy(entries[*count].node_id, sizeof(entries[*count].node_id), rn_trim(node));
        (*count)++;
    }
    for (size_t i = 0; i < n; i++) free(lines[i]);
    free(lines);
    *out = entries;
    return 0;
}

int rn_config_load_users(const char *path, rn_user_entry **out, size_t *count) {
    *out = NULL;
    *count = 0;
    char **lines = NULL;
    size_t n = 0;
    if (load_lines(path, &lines, &n) != 0) return -1;
    rn_user_entry *entries = NULL;
    for (size_t i = 0; i < n; i++) {
        char *save = NULL;
        char *user_id = strtok_r(lines[i], "|", &save);
        char *alias = strtok_r(NULL, "|", &save);
        char *home = strtok_r(NULL, "|", &save);
        char *state = strtok_r(NULL, "|", &save);
        char *created = strtok_r(NULL, "|", &save);
        char *updated = strtok_r(NULL, "|", &save);
        if (!user_id || !alias || !home || !state || !created || !updated) continue;
        entries = rn_xrealloc(entries, sizeof(*entries) * (*count + 1));
        rn_safe_copy(entries[*count].user_id, sizeof(entries[*count].user_id), rn_trim(user_id));
        rn_safe_copy(entries[*count].alias, sizeof(entries[*count].alias), rn_trim(alias));
        rn_safe_copy(entries[*count].home_node, sizeof(entries[*count].home_node), rn_trim(home));
        rn_safe_copy(entries[*count].state, sizeof(entries[*count].state), rn_trim(state));
        rn_safe_copy(entries[*count].created_at, sizeof(entries[*count].created_at), rn_trim(created));
        rn_safe_copy(entries[*count].updated_at, sizeof(entries[*count].updated_at), rn_trim(updated));
        (*count)++;
    }
    for (size_t i = 0; i < n; i++) free(lines[i]);
    free(lines);
    *out = entries;
    return 0;
}

int rn_config_load_trust(const char *path, rn_trust_entry **out, size_t *count) {
    *out = NULL;
    *count = 0;
    char **lines = NULL;
    size_t n = 0;
    if (load_lines(path, &lines, &n) != 0) return -1;
    rn_trust_entry *entries = NULL;
    for (size_t i = 0; i < n; i++) {
        char *save = NULL;
        char *node_id = strtok_r(lines[i], "|", &save);
        char *alias = strtok_r(NULL, "|", &save);
        char *state = strtok_r(NULL, "|", &save);
        char *limit = strtok_r(NULL, "|", &save);
        if (!node_id || !alias || !state || !limit) continue;
        entries = rn_xrealloc(entries, sizeof(*entries) * (*count + 1));
        rn_safe_copy(entries[*count].node_id, sizeof(entries[*count].node_id), rn_trim(node_id));
        rn_safe_copy(entries[*count].node_alias, sizeof(entries[*count].node_alias), rn_trim(alias));
        rn_safe_copy(entries[*count].state, sizeof(entries[*count].state), rn_trim(state));
        entries[*count].reg_limit_per_day = atoi(rn_trim(limit));
        (*count)++;
    }
    for (size_t i = 0; i < n; i++) free(lines[i]);
    free(lines);
    *out = entries;
    return 0;
}

int rn_config_load_peers(const char *path, rn_peer_entry **out, size_t *count) {
    *out = NULL;
    *count = 0;
    char **lines = NULL;
    size_t n = 0;
    if (load_lines(path, &lines, &n) != 0) return -1;
    rn_peer_entry *entries = NULL;
    for (size_t i = 0; i < n; i++) {
        char *save = NULL;
        char *node_id = strtok_r(lines[i], "|", &save);
        char *alias = strtok_r(NULL, "|", &save);
        char *proto = strtok_r(NULL, "|", &save);
        char *host = strtok_r(NULL, "|", &save);
        char *port = strtok_r(NULL, "|", &save);
        char *status = strtok_r(NULL, "|", &save);
        if (!node_id || !alias || !proto || !host || !port || !status) continue;
        entries = rn_xrealloc(entries, sizeof(*entries) * (*count + 1));
        rn_safe_copy(entries[*count].node_id, sizeof(entries[*count].node_id), rn_trim(node_id));
        rn_safe_copy(entries[*count].node_alias, sizeof(entries[*count].node_alias), rn_trim(alias));
        rn_safe_copy(entries[*count].proto, sizeof(entries[*count].proto), rn_trim(proto));
        rn_safe_copy(entries[*count].host, sizeof(entries[*count].host), rn_trim(host));
        entries[*count].port = atoi(rn_trim(port));
        rn_safe_copy(entries[*count].status, sizeof(entries[*count].status), rn_trim(status));
        (*count)++;
    }
    for (size_t i = 0; i < n; i++) free(lines[i]);
    free(lines);
    *out = entries;
    return 0;
}

static int find_alias_entry(const char *path, const char *key, rn_alias_entry *out) {
    rn_alias_entry *entries = NULL;
    size_t count = 0;
    if (rn_config_load_aliases(path, &entries, &count) != 0) return -1;
    int found = -1;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].address, key) == 0 || strcmp(entries[i].user_id, key) == 0) {
            if (out) *out = entries[i];
            found = 0;
            break;
        }
    }
    free(entries);
    return found;
}

int rn_config_find_alias(const char *aliases_path, const char *address, rn_alias_entry *out) {
    return find_alias_entry(aliases_path, address, out);
}

int rn_config_find_user_by_id(const char *aliases_path, const char *user_id, rn_alias_entry *out) {
    return find_alias_entry(aliases_path, user_id, out);
}

static int find_user_record_entry(const char *users_path, const char *key, bool by_id, rn_user_entry *out) {
    rn_user_entry *entries = NULL;
    size_t count = 0;
    if (rn_config_load_users(users_path, &entries, &count) != 0) return -1;
    int found = -1;
    for (size_t i = 0; i < count; i++) {
        if ((by_id && strcmp(entries[i].user_id, key) == 0) || (!by_id && strcmp(entries[i].alias, key) == 0)) {
            if (out) *out = entries[i];
            found = 0;
            break;
        }
    }
    free(entries);
    return found;
}

int rn_config_find_user_record(const char *users_path, const char *alias_or_user_id, rn_user_entry *out) {
    return find_user_record_entry(users_path, alias_or_user_id, false, out) == 0 ? 0 : find_user_record_entry(users_path, alias_or_user_id, true, out);
}

int rn_config_find_user_record_by_alias(const char *users_path, const char *alias, rn_user_entry *out) {
    return find_user_record_entry(users_path, alias, false, out);
}

int rn_config_find_user_record_by_id(const char *users_path, const char *user_id, rn_user_entry *out) {
    return find_user_record_entry(users_path, user_id, true, out);
}

static int save_lines(const char *path, const char *const *lines, size_t count) {
    if (rn_ensure_parent_dir(path) != 0) return -1;
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    for (size_t i = 0; i < count; i++) {
        if (fprintf(f, "%s\n", lines[i]) < 0) {
            fclose(f);
            return -1;
        }
    }
    fclose(f);
    return 0;
}

int rn_config_save_users(const char *path, const rn_user_entry *entries, size_t count) {
    char **lines = calloc(count ? count : 1, sizeof(char *));
    if (!lines) return -1;
    for (size_t i = 0; i < count; i++) {
        int needed = snprintf(NULL, 0, "%s|%s|%s|%s|%s|%s", entries[i].user_id, entries[i].alias, entries[i].home_node,
                              entries[i].state, entries[i].created_at, entries[i].updated_at);
        lines[i] = rn_xmalloc((size_t)needed + 1);
        snprintf(lines[i], (size_t)needed + 1, "%s|%s|%s|%s|%s|%s", entries[i].user_id, entries[i].alias, entries[i].home_node,
                 entries[i].state, entries[i].created_at, entries[i].updated_at);
    }
    int rc = save_lines(path, (const char *const *)lines, count);
    for (size_t i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return rc;
}

int rn_config_save_aliases(const char *path, const rn_user_entry *entries, size_t count) {
    char **lines = calloc(count ? count : 1, sizeof(char *));
    if (!lines) return -1;
    for (size_t i = 0; i < count; i++) {
        int needed = snprintf(NULL, 0, "%s|%s|%s", entries[i].alias, entries[i].user_id, entries[i].home_node);
        lines[i] = rn_xmalloc((size_t)needed + 1);
        snprintf(lines[i], (size_t)needed + 1, "%s|%s|%s", entries[i].alias, entries[i].user_id, entries[i].home_node);
    }
    int rc = save_lines(path, (const char *const *)lines, count);
    for (size_t i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return rc;
}

int rn_config_append_registry_event(const char *path, const char *line) {
    if (!line) return -1;
    FILE *f = fopen(path, "a");
    if (!f) return -1;
    int rc = fprintf(f, "%s\n", line) < 0 ? -1 : 0;
    fclose(f);
    return rc;
}

int rn_config_append_registry_payload(const char *dir, const char *event_id, const char *payload, size_t len) {
    char path[RN_MAX_PATH];
    if (snprintf(path, sizeof(path), "%s/%s.txt", dir, event_id) >= (int)sizeof(path)) return -1;
    return rn_write_entire_file(path, payload, len);
}

int rn_config_next_user_id(const char *users_path, const char *home_node, char *out, size_t out_sz) {
    rn_user_entry *users = NULL;
    size_t count = 0;
    if (rn_config_load_users(users_path, &users, &count) != 0) return -1;
    unsigned next = 1;
    char compact[RN_MAX_FIELD];
    size_t j = 0;
    for (size_t i = 0; home_node[i] && j + 1 < sizeof(compact); i++) {
        if (home_node[i] != '-') compact[j++] = home_node[i];
    }
    compact[j] = '\0';
    char prefix[RN_MAX_FIELD];
    if (snprintf(prefix, sizeof(prefix), "USR-%s-", compact) >= (int)sizeof(prefix)) {
        free(users);
        return -1;
    }
    for (size_t i = 0; i < count; i++) {
        if (!rn_starts_with(users[i].user_id, prefix)) continue;
        const char *dash = strrchr(users[i].user_id, '-');
        if (!dash) continue;
        unsigned seq = (unsigned)atoi(dash + 1);
        if (seq >= next) next = seq + 1;
    }
    free(users);
    return snprintf(out, out_sz, "USR-%s-%06u", compact, next) >= (int)out_sz ? -1 : 0;
}

int rn_config_find_trust(const char *trust_path, const char *node_id, rn_trust_entry *out) {
    rn_trust_entry *entries = NULL;
    size_t count = 0;
    if (rn_config_load_trust(trust_path, &entries, &count) != 0) return -1;
    int found = -1;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].node_id, node_id) == 0) {
            if (out) *out = entries[i];
            found = 0;
            break;
        }
    }
    free(entries);
    return found;
}

bool rn_user_state_is_active(const char *state) {
    return state && strcmp(state, "active") == 0;
}

bool rn_user_state_is_terminal(const char *state) {
    return state && (strcmp(state, "rejected") == 0 || strcmp(state, "conflict") == 0 || strcmp(state, "revoked") == 0);
}

bool rn_trust_state_is_blocked(const char *state) {
    return state && strcmp(state, "blocked") == 0;
}

int rn_config_find_peer(const char *peers_path, const char *node_id, rn_peer_entry *out) {
    rn_peer_entry *entries = NULL;
    size_t count = 0;
    if (rn_config_load_peers(peers_path, &entries, &count) != 0) return -1;
    int found = -1;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].node_id, node_id) == 0) {
            if (out) *out = entries[i];
            found = 0;
            break;
        }
    }
    free(entries);
    return found;
}
