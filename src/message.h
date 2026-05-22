#ifndef RETRONET_MESSAGE_H
#define RETRONET_MESSAGE_H

#include "common.h"

typedef struct {
    char to[RN_MAX_FIELD];
    char from[RN_MAX_FIELD];
    char msg_id[RN_MAX_FIELD];
    char date[RN_MAX_FIELD];
    char source_node[RN_MAX_FIELD];
    char dest_user[RN_MAX_FIELD];
    char dest_node[RN_MAX_FIELD];
    char status[RN_MAX_FIELD];
    char body[RN_MAX_BODY];
} rn_message;

typedef struct {
    char type[RN_MAX_FIELD];
    char alias[RN_MAX_FIELD];
    char user_id[RN_MAX_FIELD];
    char home_node[RN_MAX_FIELD];
    char created_by[RN_MAX_FIELD];
    char reason[RN_MAX_FIELD];
    char existing_user_id[RN_MAX_FIELD];
    char existing_home_node[RN_MAX_FIELD];
    char local_user_id[RN_MAX_FIELD];
    char remote_user_id[RN_MAX_FIELD];
} rn_registry_event;

void rn_message_init(rn_message *msg);
int rn_message_from_user_payload(const char *payload, char *to, size_t to_sz, char *body, size_t body_sz);
int rn_message_parse_draft(const char *payload, char *to, size_t to_sz, char *body, size_t body_sz);
void rn_message_fill_auto(rn_message *msg, const char *to, const char *from, const char *msg_id, const char *date,
                          const char *source_node, const char *dest_user, const char *dest_node, const char *status,
                          const char *body);
int rn_message_generate_id(char *out, size_t out_sz);
int rn_message_serialize(const rn_message *msg, char **out, size_t *len_out);
int rn_message_write_file(const char *path, const rn_message *msg);
int rn_message_read_file(const char *path, rn_message *msg);
int rn_message_print(const rn_message *msg, FILE *fp);
void rn_registry_event_init(rn_registry_event *ev);
int rn_registry_event_parse(const char *text, rn_registry_event *ev);
int rn_registry_event_serialize(const rn_registry_event *ev, char **out, size_t *len_out);
int rn_registry_event_type_is_user_event(const char *type);

#endif
