#include "spool.h"

static int path_cmp(const void *a, const void *b) {
    const char *pa = *(const char *const *)a;
    const char *pb = *(const char *const *)b;
    return strcmp(pa, pb);
}

int rn_spool_prepare_node(const char *node_root) {
    char path[RN_MAX_PATH];
    if (rn_join_path(path, sizeof(path), node_root, "config") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join_path(path, sizeof(path), node_root, "spool") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "users") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "peers") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "transit") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "deadletter") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "registry") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    if (rn_join3_path(path, sizeof(path), node_root, "spool", "quarantine") != 0) return -1;
    if (rn_mkdir_p(path) != 0) return -1;
    return 0;
}

int rn_spool_store_local_message(const char *node_root, const char *user_id, const rn_message *msg, char *out_path, size_t out_sz) {
    char base[RN_MAX_PATH];
    char dir[RN_MAX_PATH];
    if (rn_join3_path(base, sizeof(base), node_root, "spool", "users") != 0) return -1;
    if (snprintf(dir, sizeof(dir), "%s/%s/inbox", base, user_id) >= (int)sizeof(dir)) return -1;
    if (rn_mkdir_p(dir) != 0) return -1;
    char msg_path[RN_MAX_PATH];
    if (rn_join_path(msg_path, sizeof(msg_path), dir, msg->msg_id) != 0) return -1;
    if (!rn_ends_with(msg_path, ".msg")) {
        if (strlen(msg_path) + 4 >= sizeof(msg_path)) return -1;
        strcat(msg_path, ".msg");
    }
    if (rn_message_write_file(msg_path, msg) != 0) return -1;
    if (out_path) rn_safe_copy(out_path, out_sz, msg_path);
    return 0;
}

int rn_spool_store_peer_message(const char *node_root, const char *peer_node_id, const rn_message *msg, char *out_path, size_t out_sz) {
    char base[RN_MAX_PATH];
    char peer_dir[RN_MAX_PATH];
    if (rn_join3_path(base, sizeof(base), node_root, "spool", "peers") != 0) return -1;
    if (snprintf(peer_dir, sizeof(peer_dir), "%s/%s/outbox", base, peer_node_id) >= (int)sizeof(peer_dir)) return -1;
    if (rn_mkdir_p(peer_dir) != 0) return -1;
    char path[RN_MAX_PATH];
    if (rn_join_path(path, sizeof(path), peer_dir, msg->msg_id) != 0) return -1;
    if (!rn_ends_with(path, ".msg")) {
        if (strlen(path) + 4 >= sizeof(path)) return -1;
        strcat(path, ".msg");
    }
    if (rn_message_write_file(path, msg) != 0) return -1;
    if (out_path) rn_safe_copy(out_path, out_sz, path);
    return 0;
}

int rn_spool_store_client_message(const char *client_root, const rn_message *msg, char *out_path, size_t out_sz) {
    char dir[RN_MAX_PATH];
    if (rn_join_path(dir, sizeof(dir), client_root, "inbox") != 0) return -1;
    if (rn_mkdir_p(dir) != 0) return -1;
    char path[RN_MAX_PATH];
    if (rn_join_path(path, sizeof(path), dir, msg->msg_id) != 0) return -1;
    if (!rn_ends_with(path, ".msg")) {
        if (strlen(path) + 4 >= sizeof(path)) return -1;
        strcat(path, ".msg");
    }
    if (rn_message_write_file(path, msg) != 0) return -1;
    if (out_path) rn_safe_copy(out_path, out_sz, path);
    return 0;
}

static int spool_store_named_text(const char *base_dir, const char *name, const char *text) {
    char dir[RN_MAX_PATH];
    if (rn_mkdir_p(base_dir) != 0) return -1;
    if (snprintf(dir, sizeof(dir), "%s", base_dir) >= (int)sizeof(dir)) return -1;
    char path[RN_MAX_PATH];
    if (snprintf(path, sizeof(path), "%s/%s", dir, name) >= (int)sizeof(path)) return -1;
    return rn_write_entire_file(path, text, strlen(text));
}

int rn_spool_store_deadletter_text(const char *node_root, const char *name, const char *text) {
    char base[RN_MAX_PATH];
    if (rn_join3_path(base, sizeof(base), node_root, "spool", "deadletter") != 0) return -1;
    return spool_store_named_text(base, name, text);
}

int rn_spool_store_registry_text(const char *node_root, const char *name, const char *text) {
    char base[RN_MAX_PATH];
    if (rn_join3_path(base, sizeof(base), node_root, "spool", "registry") != 0) return -1;
    return spool_store_named_text(base, name, text);
}

int rn_spool_store_quarantine_text(const char *node_root, const char *name, const char *text) {
    char base[RN_MAX_PATH];
    if (rn_join3_path(base, sizeof(base), node_root, "spool", "quarantine") != 0) return -1;
    return spool_store_named_text(base, name, text);
}

int rn_spool_list_directory(const char *dir, char ***paths_out, size_t *count_out) {
    *paths_out = NULL;
    *count_out = 0;
    DIR *d = opendir(dir);
    if (!d) return -1;
    struct dirent *de;
    char **paths = NULL;
    size_t count = 0;
    while ((de = readdir(d)) != NULL) {
        if (de->d_name[0] == '.') continue;
        char path[RN_MAX_PATH];
        if (rn_join_path(path, sizeof(path), dir, de->d_name) != 0) continue;
        struct stat st;
        if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) continue;
        paths = rn_xrealloc(paths, sizeof(char *) * (count + 1));
        paths[count++] = rn_xstrdup(path);
    }
    closedir(d);
    qsort(paths, count, sizeof(char *), path_cmp);
    *paths_out = paths;
    *count_out = count;
    return 0;
}

int rn_spool_load_message_paths(const char *dir, char ***paths_out, size_t *count_out) {
    return rn_spool_list_directory(dir, paths_out, count_out);
}

int rn_spool_move_file(const char *src, const char *dst) {
    if (rename(src, dst) == 0) return 0;
    if (rn_copy_file(src, dst) != 0) return -1;
    return remove(src);
}
