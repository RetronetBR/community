#ifndef RETRONET_NET_H
#define RETRONET_NET_H

#include "common.h"

int rn_net_listen(const char *host, int port);
int rn_net_connect(const char *host, int port);
int rn_net_read_line(int fd, char *buf, size_t buf_sz);
int rn_net_write_all(int fd, const void *buf, size_t len);
int rn_net_write_line(int fd, const char *line);
int rn_net_read_exact(int fd, void *buf, size_t len);
void rn_net_close(int fd);

#endif
