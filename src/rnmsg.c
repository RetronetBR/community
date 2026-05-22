#include "spool.h"
#include <sys/wait.h>

static void usage(void) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  rnmsg draft <client_root> <to> <body...>\n");
    fprintf(stderr, "  rnmsg send <client_root> <draft_file> [--sync]\n");
    fprintf(stderr, "  rnmsg reply <client_root> <inbox_message> <body...>\n");
    fprintf(stderr, "  rnmsg sync <client_root>\n");
    fprintf(stderr, "  rnmsg --sync <client_root>\n");
    fprintf(stderr, "  rnmsg read <path>\n");
}

static int spawn_tool(const char *argv0, char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        execvp(argv0, argv);
        _exit(127);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return -1;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}

static int sibling_tool_path(const char *self, const char *tool, char *out, size_t out_sz) {
    const char *slash = strrchr(self, '/');
    if (!slash) {
        return snprintf(out, out_sz, "%s", tool) >= (int)out_sz ? -1 : 0;
    }
    size_t dir_len = (size_t)(slash - self);
    if (dir_len + 1 + strlen(tool) + 1 > out_sz) return -1;
    memcpy(out, self, dir_len);
    out[dir_len] = '\0';
    strcat(out, "/");
    strcat(out, tool);
    return 0;
}

static int resolve_node_root(const rn_client_config *client, char *out, size_t out_sz) {
    return rn_resolve_path(out, out_sz, client->root_dir, client->node_root);
}

static int sync_client(const char *self, const rn_client_config *client, bool pull_too) {
    char rnsync_path[RN_MAX_PATH];
    char node_root[RN_MAX_PATH];
    if (sibling_tool_path(self, "rnsync", rnsync_path, sizeof(rnsync_path)) != 0) return -1;
    if (resolve_node_root(client, node_root, sizeof(node_root)) != 0) return -1;

    char *push_argv[] = {rnsync_path, "push", (char *)client->root_dir, node_root, NULL};
    char *pull_argv[] = {rnsync_path, "pull", (char *)client->root_dir, node_root, NULL};
    if (spawn_tool(rnsync_path, push_argv) != 0) return -1;
    if (pull_too || client->auto_pull || client->auto_sync) {
        if (spawn_tool(rnsync_path, pull_argv) != 0) return -1;
    }
    return 0;
}

static int read_text_file(const char *path, char **out) {
    size_t len = 0;
    return rn_read_entire_file(path, out, &len);
}

static int create_draft(const rn_client_config *client, const char *to, int argc, char **argv) {
    if (rn_mkdir_p(client->drafts_dir) != 0) return -1;
    char body[RN_MAX_BODY];
    body[0] = '\0';
    for (int i = 0; i < argc; i++) {
        if (i > 0 && strlen(body) + 1 < sizeof(body)) strcat(body, " ");
        if (strlen(body) + strlen(argv[i]) < sizeof(body)) strcat(body, argv[i]);
    }
    char msg_id[RN_MAX_FIELD];
    if (rn_message_generate_id(msg_id, sizeof(msg_id)) != 0) return -1;
    char draft_path[RN_MAX_PATH];
    if (snprintf(draft_path, sizeof(draft_path), "%s/%s.txt", client->drafts_dir, msg_id) >= (int)sizeof(draft_path)) return -1;
    char *payload = NULL;
    int needed = snprintf(NULL, 0, "to: %s\n---\n%s\n", to, body);
    if (needed < 0) return -1;
    payload = rn_xmalloc((size_t)needed + 1);
    snprintf(payload, (size_t)needed + 1, "to: %s\n---\n%s\n", to, body);
    int rc = rn_write_entire_file(draft_path, payload, (size_t)needed);
    free(payload);
    if (rc == 0) printf("%s\n", draft_path);
    return rc;
}

static int read_message_file(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    if (S_ISDIR(st.st_mode)) {
        char **files = NULL;
        size_t count = 0;
        if (rn_spool_load_message_paths(path, &files, &count) != 0) return -1;
        for (size_t i = 0; i < count; i++) {
            int rc = read_message_file(files[i]);
            free(files[i]);
            if (rc != 0) {
                free(files);
                return rc;
            }
        }
        free(files);
        return 0;
    }
    rn_message msg;
    if (rn_message_read_file(path, &msg) == 0) {
        return rn_message_print(&msg, stdout);
    }
    char *buf = NULL;
    if (read_text_file(path, &buf) != 0) return -1;
    fputs(buf, stdout);
    if (buf[0] != '\0' && buf[strlen(buf) - 1] != '\n') fputc('\n', stdout);
    free(buf);
    return 0;
}

static int send_draft(const char *self, rn_client_config *client, const char *draft_path, bool force_sync) {
    char *buf = NULL;
    if (read_text_file(draft_path, &buf) != 0) return -1;

    char to[RN_MAX_FIELD];
    char body[RN_MAX_BODY];
    if (rn_message_parse_draft(buf, to, sizeof(to), body, sizeof(body)) != 0) {
        free(buf);
        return -1;
    }
    free(buf);

    rn_node_config node;
    char node_root[RN_MAX_PATH];
    if (resolve_node_root(client, node_root, sizeof(node_root)) != 0) return -1;
    if (rn_config_load_node(node_root, &node) != 0) return -1;

    rn_route_result route;
    if (rn_route_resolve(&node, to, &route) != 0) return -1;

    rn_alias_entry sender_alias;
    if (rn_route_sender_lookup(&node, client->user_id, &sender_alias) != 0) return -1;

    rn_message msg;
    rn_message_fill_auto(&msg, to, sender_alias.address, "", "", sender_alias.node_id, route.alias.user_id,
                         route.alias.node_id, "DRAFT", body);
    if (rn_message_generate_id(msg.msg_id, sizeof(msg.msg_id)) != 0) return -1;
    if (rn_now_iso8601(msg.date, sizeof(msg.date)) != 0) return -1;
    rn_safe_copy(msg.status, sizeof(msg.status), "QUEUED");

    char outbox_dir[RN_MAX_PATH];
    if (rn_join_path(outbox_dir, sizeof(outbox_dir), client->root_dir, "outbox") != 0) return -1;
    if (rn_mkdir_p(outbox_dir) != 0) return -1;
    char outbox_file[RN_MAX_PATH];
    if (snprintf(outbox_file, sizeof(outbox_file), "%s/%s.msg", outbox_dir, msg.msg_id) >= (int)sizeof(outbox_file)) return -1;
    if (rn_message_write_file(outbox_file, &msg) != 0) return -1;

    printf("%s\n", outbox_file);
    if (sync_client(self, client, force_sync || client->auto_sync || client->auto_pull) != 0) return -1;
    return 0;
}

static int reply_to_message(const char *self, const char *client_root, const char *message_path, int argc, char **argv) {
    rn_client_config client;
    if (rn_config_load_client(client_root, &client) != 0) return -1;
    rn_message original;
    if (rn_message_read_file(message_path, &original) != 0) return -1;
    if (original.from[0] == '\0') return -1;

    char body[RN_MAX_BODY];
    body[0] = '\0';
    for (int i = 0; i < argc; i++) {
        if (i > 0 && strlen(body) + 1 < sizeof(body)) strcat(body, " ");
        if (strlen(body) + strlen(argv[i]) < sizeof(body)) strcat(body, argv[i]);
    }

    char draft_path[RN_MAX_PATH];
    if (rn_mkdir_p(client.drafts_dir) != 0) return -1;
    if (snprintf(draft_path, sizeof(draft_path), "%s/reply-%s.txt", client.drafts_dir, original.msg_id) >= (int)sizeof(draft_path)) return -1;
    char *payload = NULL;
    int needed = snprintf(NULL, 0, "to: %s\n---\n%s\n", original.from, body);
    if (needed < 0) return -1;
    payload = rn_xmalloc((size_t)needed + 1);
    snprintf(payload, (size_t)needed + 1, "to: %s\n---\n%s\n", original.from, body);
    int rc = rn_write_entire_file(draft_path, payload, (size_t)needed);
    free(payload);
    if (rc != 0) return -1;
    return send_draft(self, &client, draft_path, true);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "--sync") == 0) {
        if (argc != 3) {
            usage();
            return 1;
        }
        rn_client_config client;
        if (rn_config_load_client(argv[2], &client) != 0) return 1;
        return sync_client(argv[0], &client, true) == 0 ? 0 : 1;
    }

    if (strcmp(argv[1], "sync") == 0) {
        if (argc != 3) {
            usage();
            return 1;
        }
        rn_client_config client;
        if (rn_config_load_client(argv[2], &client) != 0) return 1;
        return sync_client(argv[0], &client, true) == 0 ? 0 : 1;
    }

    if (strcmp(argv[1], "draft") == 0) {
        if (argc < 5) {
            usage();
            return 1;
        }
        rn_client_config client;
        if (rn_config_load_client(argv[2], &client) != 0) return 1;
        return create_draft(&client, argv[3], argc - 4, &argv[4]) == 0 ? 0 : 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        if (argc < 4) {
            usage();
            return 1;
        }
        rn_client_config client;
        if (rn_config_load_client(argv[2], &client) != 0) return 1;
        bool force_sync = false;
        const char *draft_path = argv[3];
        if (argc > 4 && strcmp(argv[4], "--sync") == 0) force_sync = true;
        return send_draft(argv[0], &client, draft_path, force_sync) == 0 ? 0 : 1;
    }

    if (strcmp(argv[1], "reply") == 0) {
        if (argc < 5) {
            usage();
            return 1;
        }
        return reply_to_message(argv[0], argv[2], argv[3], argc - 4, &argv[4]) == 0 ? 0 : 1;
    }

    if (strcmp(argv[1], "read") == 0) {
        if (argc != 3) {
            usage();
            return 1;
        }
        return read_message_file(argv[2]) == 0 ? 0 : 1;
    }

    usage();
    return 1;
}
