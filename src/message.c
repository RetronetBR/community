#include "message.h"

void rn_message_init(rn_message *msg) {
    memset(msg, 0, sizeof(*msg));
}

int rn_message_from_user_payload(const char *payload, char *to, size_t to_sz, char *body, size_t body_sz) {
    return rn_message_parse_draft(payload, to, to_sz, body, body_sz);
}

int rn_message_parse_draft(const char *payload, char *to, size_t to_sz, char *body, size_t body_sz) {
    if (!payload || !to || !body) return -1;
    to[0] = '\0';
    body[0] = '\0';
    char *copy = rn_xstrdup(payload);
    char *sep = strstr(copy, "\n---\n");
    if (!sep) sep = strstr(copy, "\r\n---\r\n");
    if (!sep) {
        free(copy);
        return -1;
    }
    *sep = '\0';
    char *header = rn_trim(copy);
    size_t sep_len = (sep[0] == '\r' && sep[1] == '\n') ? 7 : 5;
    char *body_start = sep + sep_len;
    while (*body_start == '\r' || *body_start == '\n') body_start++;
    char *colon = strchr(header, ':');
    if (!colon) {
        free(copy);
        return -1;
    }
    *colon = '\0';
    char *key = rn_trim(header);
    char *value = rn_trim(colon + 1);
    if (strcasecmp(key, "to") != 0) {
        free(copy);
        return -1;
    }
    rn_safe_copy(to, to_sz, value);
    size_t body_len = strlen(body_start);
    if (body_len >= body_sz) body_len = body_sz - 1;
    memcpy(body, body_start, body_len);
    body[body_len] = '\0';
    rn_rstrip(body);
    free(copy);
    return 0;
}

void rn_message_fill_auto(rn_message *msg, const char *to, const char *from, const char *msg_id, const char *date,
                          const char *source_node, const char *dest_user, const char *dest_node, const char *status,
                          const char *body) {
    rn_message_init(msg);
    rn_safe_copy(msg->to, sizeof(msg->to), to);
    rn_safe_copy(msg->from, sizeof(msg->from), from);
    rn_safe_copy(msg->msg_id, sizeof(msg->msg_id), msg_id);
    rn_safe_copy(msg->date, sizeof(msg->date), date);
    rn_safe_copy(msg->source_node, sizeof(msg->source_node), source_node);
    rn_safe_copy(msg->dest_user, sizeof(msg->dest_user), dest_user);
    rn_safe_copy(msg->dest_node, sizeof(msg->dest_node), dest_node);
    rn_safe_copy(msg->status, sizeof(msg->status), status);
    rn_safe_copy(msg->body, sizeof(msg->body), body);
}

int rn_message_generate_id(char *out, size_t out_sz) {
    static unsigned long seq = 0;
    time_t now = time(NULL);
    struct tm tmv;
    if (!localtime_r(&now, &tmv)) return -1;
    char stamp[32];
    if (strftime(stamp, sizeof(stamp), "%Y%m%d%H%M%S", &tmv) == 0) return -1;
    seq++;
    return snprintf(out, out_sz, "MSG-%s-%ld-%lu", stamp, (long)getpid(), seq) >= (int)out_sz ? -1 : 0;
}

int rn_message_serialize(const rn_message *msg, char **out, size_t *len_out) {
    const char *fmt =
        "To: %s\n"
        "From: %s\n"
        "MsgID: %s\n"
        "Date: %s\n"
        "SourceNode: %s\n"
        "DestUser: %s\n"
        "DestNode: %s\n"
        "Status: %s\n"
        "---\n"
        "%s";
    int needed = snprintf(NULL, 0, fmt, msg->to, msg->from, msg->msg_id, msg->date, msg->source_node,
                          msg->dest_user, msg->dest_node, msg->status, msg->body);
    if (needed < 0) return -1;
    char *buf = rn_xmalloc((size_t)needed + 1);
    snprintf(buf, (size_t)needed + 1, fmt, msg->to, msg->from, msg->msg_id, msg->date, msg->source_node,
             msg->dest_user, msg->dest_node, msg->status, msg->body);
    *out = buf;
    if (len_out) *len_out = (size_t)needed;
    return 0;
}

int rn_message_write_file(const char *path, const rn_message *msg) {
    char *buf = NULL;
    size_t len = 0;
    if (rn_message_serialize(msg, &buf, &len) != 0) return -1;
    int rc = rn_write_entire_file(path, buf, len);
    free(buf);
    return rc;
}

static int parse_header_line(const char *line, rn_message *msg) {
    const char *sep = strstr(line, ": ");
    if (!sep) return 0;
    size_t key_len = (size_t)(sep - line);
    const char *value = sep + 2;
    if (strncmp(line, "To", key_len) == 0) rn_safe_copy(msg->to, sizeof(msg->to), value);
    else if (strncmp(line, "From", key_len) == 0) rn_safe_copy(msg->from, sizeof(msg->from), value);
    else if (strncmp(line, "MsgID", key_len) == 0) rn_safe_copy(msg->msg_id, sizeof(msg->msg_id), value);
    else if (strncmp(line, "Date", key_len) == 0) rn_safe_copy(msg->date, sizeof(msg->date), value);
    else if (strncmp(line, "SourceNode", key_len) == 0) rn_safe_copy(msg->source_node, sizeof(msg->source_node), value);
    else if (strncmp(line, "DestUser", key_len) == 0) rn_safe_copy(msg->dest_user, sizeof(msg->dest_user), value);
    else if (strncmp(line, "DestNode", key_len) == 0) rn_safe_copy(msg->dest_node, sizeof(msg->dest_node), value);
    else if (strncmp(line, "Status", key_len) == 0) rn_safe_copy(msg->status, sizeof(msg->status), value);
    return 0;
}

int rn_message_read_file(const char *path, rn_message *msg) {
    rn_message_init(msg);
    char *buf = NULL;
    size_t len = 0;
    if (rn_read_entire_file(path, &buf, &len) != 0) return -1;
    char *sep = strstr(buf, "\n---\n");
    if (!sep) {
        free(buf);
        return -1;
    }
    *sep = '\0';
    char *headers = buf;
    char *body = sep + 5;
    char *save = NULL;
    for (char *line = strtok_r(headers, "\n", &save); line; line = strtok_r(NULL, "\n", &save)) {
        rn_rstrip(line);
        line = rn_trim(line);
        if (*line == '\0') continue;
        parse_header_line(line, msg);
    }
    rn_safe_copy(msg->body, sizeof(msg->body), body);
    free(buf);
    return 0;
}

int rn_message_print(const rn_message *msg, FILE *fp) {
    return fprintf(fp,
                   "To: %s\nFrom: %s\nMsgID: %s\nDate: %s\nSourceNode: %s\nDestUser: %s\nDestNode: %s\nStatus: %s\n---\n%s\n",
                   msg->to, msg->from, msg->msg_id, msg->date, msg->source_node, msg->dest_user, msg->dest_node,
                   msg->status, msg->body) < 0 ? -1 : 0;
}

void rn_registry_event_init(rn_registry_event *ev) {
    memset(ev, 0, sizeof(*ev));
}

static void set_event_field(rn_registry_event *ev, const char *key, const char *value) {
    if (strcmp(key, "Type") == 0) rn_safe_copy(ev->type, sizeof(ev->type), value);
    else if (strcmp(key, "Alias") == 0) rn_safe_copy(ev->alias, sizeof(ev->alias), value);
    else if (strcmp(key, "UserID") == 0) rn_safe_copy(ev->user_id, sizeof(ev->user_id), value);
    else if (strcmp(key, "HomeNode") == 0) rn_safe_copy(ev->home_node, sizeof(ev->home_node), value);
    else if (strcmp(key, "CreatedBy") == 0) rn_safe_copy(ev->created_by, sizeof(ev->created_by), value);
    else if (strcmp(key, "Reason") == 0) rn_safe_copy(ev->reason, sizeof(ev->reason), value);
    else if (strcmp(key, "ExistingUserID") == 0) rn_safe_copy(ev->existing_user_id, sizeof(ev->existing_user_id), value);
    else if (strcmp(key, "ExistingHomeNode") == 0) rn_safe_copy(ev->existing_home_node, sizeof(ev->existing_home_node), value);
    else if (strcmp(key, "LocalUserID") == 0) rn_safe_copy(ev->local_user_id, sizeof(ev->local_user_id), value);
    else if (strcmp(key, "RemoteUserID") == 0) rn_safe_copy(ev->remote_user_id, sizeof(ev->remote_user_id), value);
}

int rn_registry_event_parse(const char *text, rn_registry_event *ev) {
    rn_registry_event_init(ev);
    char *copy = rn_xstrdup(text);
    char *sep = strstr(copy, "\n---\n");
    if (!sep) {
        free(copy);
        return -1;
    }
    *sep = '\0';
    char *headers = copy;
    char *body = sep + 5;
    char *save = NULL;
    for (char *line = strtok_r(headers, "\n", &save); line; line = strtok_r(NULL, "\n", &save)) {
        rn_rstrip(line);
        line = rn_trim(line);
        if (*line == '\0') continue;
        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = '\0';
        set_event_field(ev, rn_trim(line), rn_trim(colon + 1));
    }
    char *save2 = NULL;
    for (char *line = strtok_r(body, "\n", &save2); line; line = strtok_r(NULL, "\n", &save2)) {
        rn_rstrip(line);
        line = rn_trim(line);
        if (*line == '\0') continue;
        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = '\0';
        set_event_field(ev, rn_trim(line), rn_trim(colon + 1));
    }
    free(copy);
    return ev->type[0] ? 0 : -1;
}

int rn_registry_event_serialize(const rn_registry_event *ev, char **out, size_t *len_out) {
    const char *fmt =
        "Type: %s\n"
        "---\n"
        "Alias: %s\n"
        "UserID: %s\n"
        "HomeNode: %s\n"
        "CreatedBy: %s\n"
        "Reason: %s\n"
        "ExistingUserID: %s\n"
        "ExistingHomeNode: %s\n"
        "LocalUserID: %s\n"
        "RemoteUserID: %s\n";
    int needed = snprintf(NULL, 0, fmt, ev->type, ev->alias, ev->user_id, ev->home_node, ev->created_by, ev->reason,
                          ev->existing_user_id, ev->existing_home_node, ev->local_user_id, ev->remote_user_id);
    if (needed < 0) return -1;
    char *buf = rn_xmalloc((size_t)needed + 1);
    snprintf(buf, (size_t)needed + 1, fmt, ev->type, ev->alias, ev->user_id, ev->home_node, ev->created_by, ev->reason,
             ev->existing_user_id, ev->existing_home_node, ev->local_user_id, ev->remote_user_id);
    *out = buf;
    if (len_out) *len_out = (size_t)needed;
    return 0;
}

int rn_registry_event_type_is_user_event(const char *type) {
    return type && (strcmp(type, "USER_REGISTER") == 0 || strcmp(type, "USER_ACCEPT") == 0 ||
                    strcmp(type, "USER_REJECT") == 0 || strcmp(type, "USER_CONFLICT") == 0 ||
                    strcmp(type, "USER_REVOKE") == 0 || strcmp(type, "USER_QUARANTINE") == 0);
}
