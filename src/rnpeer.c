#include "spool.h"
#include "net.h"

static void usage(void) {
    fprintf(stderr, "usage: rnpeer push <source_node_root> <dest_node_root>\n");
}

static int push_peer_file(const rn_node_config *src, const rn_node_config *dst, const char *path) {
    char *payload = NULL;
    size_t len = 0;
    if (rn_read_entire_file(path, &payload, &len) != 0) return -1;
    int fd = rn_net_connect(dst->host, dst->port);
    if (fd < 0) {
        free(payload);
        return -1;
    }
    char line[RN_MAX_LINE];
    snprintf(line, sizeof(line), "HELLO %s", src->node_id);
    int rc = 0;
    if (rn_net_write_line(fd, line) != 0 ||
        rn_net_write_line(fd, "PUSH") != 0 ||
        snprintf(line, sizeof(line), "SIZE %zu", len) >= (int)sizeof(line) ||
        rn_net_write_line(fd, line) != 0 ||
        rn_net_write_all(fd, payload, len) != 0 ||
        rn_net_write_line(fd, "END") != 0) {
        rc = -1;
    } else if (rn_net_read_line(fd, line, sizeof(line)) <= 0 || !rn_starts_with(rn_trim(line), "OK ")) {
        rc = -1;
    }
    rn_net_close(fd);
    if (rc == 0) {
        char sent_path[RN_MAX_PATH];
        snprintf(sent_path, sizeof(sent_path), "%s.sent", path);
        rename(path, sent_path);
    }
    free(payload);
    return rc;
}

static int push_registry_file(const rn_node_config *src, const rn_node_config *dst, const char *path) {
    return push_peer_file(src, dst, path);
}

int main(int argc, char **argv) {
    if (argc != 4 || strcmp(argv[1], "push") != 0) {
        usage();
        return 1;
    }
    rn_node_config src;
    rn_node_config dst;
    if (rn_config_load_node(argv[2], &src) != 0 || rn_config_load_node(argv[3], &dst) != 0) {
        fprintf(stderr, "rnpeer: failed to load config\n");
        return 1;
    }
    char peer_dir[RN_MAX_PATH];
    if (snprintf(peer_dir, sizeof(peer_dir), "%s/peers/%s/outbox", src.spool_dir, dst.node_id) >= (int)sizeof(peer_dir)) return 1;
    char **files = NULL;
    size_t count = 0;
    if (rn_spool_load_message_paths(peer_dir, &files, &count) == 0) {
        int rc = 0;
        for (size_t i = 0; i < count; i++) {
            if (rn_ends_with(files[i], ".sent")) continue;
            if (push_peer_file(&src, &dst, files[i]) != 0) rc = 1;
            free(files[i]);
        }
        free(files);
        if (rc != 0) return rc;
    }
    char registry_dir[RN_MAX_PATH];
    if (snprintf(registry_dir, sizeof(registry_dir), "%s/registry", src.spool_dir) >= (int)sizeof(registry_dir)) return 1;
    files = NULL;
    count = 0;
    if (rn_spool_load_message_paths(registry_dir, &files, &count) == 0) {
        int rc = 0;
        for (size_t i = 0; i < count; i++) {
            if (rn_ends_with(files[i], ".sent")) continue;
            if (push_registry_file(&src, &dst, files[i]) != 0) rc = 1;
            free(files[i]);
        }
        free(files);
        if (rc != 0) return rc;
    }
    return 0;
}
