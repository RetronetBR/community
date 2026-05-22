#include "spool.h"
#include "net.h"

static void usage(void) {
    fprintf(stderr, "usage: rnsync push <client_root> <node_root>\n");
    fprintf(stderr, "       rnsync pull <client_root> <node_root>\n");
}

static int load_client_and_node(const char *client_root, const char *node_root, rn_client_config *client, rn_node_config *node) {
    if (rn_config_load_client(client_root, client) != 0) return -1;
    if (rn_config_load_node(node_root, node) != 0) return -1;
    return 0;
}

static int send_payload(int fd, const char *client_id, const char *payload, size_t len) {
    char line[RN_MAX_LINE];
    snprintf(line, sizeof(line), "HELLO %s", client_id);
    if (rn_net_write_line(fd, line) != 0) return -1;
    if (rn_net_write_line(fd, "PUSH") != 0) return -1;
    snprintf(line, sizeof(line), "SIZE %zu", len);
    if (rn_net_write_line(fd, line) != 0) return -1;
    if (rn_net_write_all(fd, payload, len) != 0) return -1;
    if (rn_net_write_line(fd, "END") != 0) return -1;
    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) return -1;
    rn_rstrip(line);
    return strncmp(line, "OK ", 3) == 0 ? 0 : -1;
}

static int push_one(const rn_client_config *client, const rn_node_config *node, const char *path) {
    char *payload = NULL;
    size_t len = 0;
    if (rn_read_entire_file(path, &payload, &len) != 0) return -1;
    int fd = rn_net_connect(node->host, node->port);
    if (fd < 0) {
        free(payload);
        return -1;
    }
    int rc = send_payload(fd, client->user_id, payload, len);
    rn_net_close(fd);
    if (rc == 0) {
        char sent_path[RN_MAX_PATH];
        snprintf(sent_path, sizeof(sent_path), "%s.sent", path);
        rename(path, sent_path);
    }
    free(payload);
    return rc;
}

static int pull_messages(const rn_client_config *client, const rn_node_config *node) {
    int fd = rn_net_connect(node->host, node->port);
    if (fd < 0) return -1;
    char line[RN_MAX_LINE];
    snprintf(line, sizeof(line), "HELLO %s", client->user_id);
    if (rn_net_write_line(fd, line) != 0) {
        rn_net_close(fd);
        return -1;
    }
    snprintf(line, sizeof(line), "PULL %s", client->user_id);
    if (rn_net_write_line(fd, line) != 0) {
        rn_net_close(fd);
        return -1;
    }
    if (rn_net_read_line(fd, line, sizeof(line)) <= 0) {
        rn_net_close(fd);
        return -1;
    }
    rn_rstrip(line);
    size_t count = 0;
    if (sscanf(line, "COUNT %zu", &count) != 1) {
        rn_net_close(fd);
        return -1;
    }
    for (size_t i = 0; i < count; i++) {
        if (rn_net_read_line(fd, line, sizeof(line)) <= 0) break;
        rn_rstrip(line);
        size_t size = 0;
        if (sscanf(line, "SIZE %zu", &size) != 1) break;
        char *buf = rn_xmalloc(size + 1);
        if (rn_net_read_exact(fd, buf, size) != 0) {
            free(buf);
            break;
        }
        buf[size] = '\0';
        if (rn_net_read_line(fd, line, sizeof(line)) <= 0) {
            free(buf);
            break;
        }
        rn_rstrip(line);
        if (strcmp(line, "END") != 0) {
            free(buf);
            break;
        }
        rn_message msg;
        rn_message_init(&msg);
        if (strstr(buf, "From:") && strstr(buf, "MsgID:")) {
            char *copy = rn_xstrdup(buf);
            char *sep = strstr(copy, "\n---\n");
            if (sep) {
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
            }
            free(copy);
        }
        if (msg.msg_id[0] == '\0') rn_message_generate_id(msg.msg_id, sizeof(msg.msg_id));
        if (msg.date[0] == '\0') rn_now_iso8601(msg.date, sizeof(msg.date));
        if (msg.status[0] == '\0') rn_safe_copy(msg.status, sizeof(msg.status), "DELIVERED");
        char saved[RN_MAX_PATH];
        if (rn_spool_store_client_message(client->root_dir, &msg, saved, sizeof(saved)) != 0) {
            free(buf);
            continue;
        }
        free(buf);
    }
    if (rn_net_read_line(fd, line, sizeof(line)) > 0) {
        rn_rstrip(line);
    }
    rn_net_close(fd);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        usage();
        return 1;
    }
    const char *mode = argv[1];
    const char *client_root = argv[2];
    const char *node_root = argv[3];
    rn_client_config client;
    rn_node_config node;
    if (load_client_and_node(client_root, node_root, &client, &node) != 0) {
        fprintf(stderr, "rnsync: failed to load config\n");
        return 1;
    }
    if (strcmp(mode, "push") == 0) {
        char **files = NULL;
        size_t count = 0;
        if (rn_spool_load_message_paths(client.outbox_dir, &files, &count) != 0) {
            fprintf(stderr, "rnsync: no outbox\n");
            return 1;
        }
        int rc = 0;
        for (size_t i = 0; i < count; i++) {
            if (rn_ends_with(files[i], ".sent")) continue;
            if (push_one(&client, &node, files[i]) != 0) rc = 1;
            free(files[i]);
        }
        free(files);
        return rc;
    }
    if (strcmp(mode, "pull") == 0) {
        return pull_messages(&client, &node) == 0 ? 0 : 1;
    }
    usage();
    return 1;
}
