#ifndef RETRONET_COMMON_H
#define RETRONET_COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define RN_MAX_PATH 512
#define RN_MAX_LINE 1024
#define RN_MAX_BODY 65536
#define RN_MAX_SMALL 128
#define RN_MAX_FIELD 128

static inline void rn_fatal(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static inline void *rn_xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) rn_fatal("malloc");
    return p;
}

static inline void *rn_xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) rn_fatal("realloc");
    return q;
}

static inline char *rn_xstrdup(const char *s) {
    char *out = strdup(s ? s : "");
    if (!out) rn_fatal("strdup");
    return out;
}

static inline void rn_rstrip(char *s) {
    size_t len = s ? strlen(s) : 0;
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || isspace((unsigned char)s[len - 1]))) {
        s[--len] = '\0';
    }
}

static inline char *rn_lskip(char *s) {
    while (s && *s && isspace((unsigned char)*s)) s++;
    return s;
}

static inline char *rn_trim(char *s) {
    if (!s) return s;
    s = rn_lskip(s);
    rn_rstrip(s);
    return s;
}

static inline bool rn_starts_with(const char *s, const char *prefix) {
    return s && prefix && strncmp(s, prefix, strlen(prefix)) == 0;
}

static inline bool rn_ends_with(const char *s, const char *suffix) {
    if (!s || !suffix) return false;
    size_t ls = strlen(s), lf = strlen(suffix);
    return ls >= lf && strcmp(s + ls - lf, suffix) == 0;
}

static inline void rn_safe_copy(char *dst, size_t dst_sz, const char *src) {
    if (dst_sz == 0) return;
    if (!src) src = "";
    snprintf(dst, dst_sz, "%s", src);
}

static inline int rn_join_path(char *out, size_t out_sz, const char *a, const char *b) {
    if (!out || out_sz == 0 || !a || !b) return -1;
    size_t la = strlen(a);
    int need_sep = (la > 0 && a[la - 1] != '/');
    return snprintf(out, out_sz, "%s%s%s", a, need_sep ? "/" : "", b) >= (int)out_sz ? -1 : 0;
}

static inline int rn_join3_path(char *out, size_t out_sz, const char *a, const char *b, const char *c) {
    char tmp[RN_MAX_PATH];
    if (rn_join_path(tmp, sizeof(tmp), a, b) != 0) return -1;
    return rn_join_path(out, out_sz, tmp, c);
}

static inline bool rn_file_exists(const char *path) {
    struct stat st;
    return path && stat(path, &st) == 0;
}

static inline int rn_mkdir_p(const char *path) {
    if (!path || !*path) return -1;
    char tmp[RN_MAX_PATH];
    rn_safe_copy(tmp, sizeof(tmp), path);
    size_t len = strlen(tmp);
    if (len == 0) return -1;
    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0777) != 0 && errno != EEXIST) return -1;
            *p = '/';
        }
    }
    if (mkdir(tmp, 0777) != 0 && errno != EEXIST) return -1;
    return 0;
}

static inline int rn_ensure_parent_dir(const char *path) {
    char tmp[RN_MAX_PATH];
    rn_safe_copy(tmp, sizeof(tmp), path);
    char *slash = strrchr(tmp, '/');
    if (!slash) return 0;
    *slash = '\0';
    return rn_mkdir_p(tmp);
}

static inline int rn_read_entire_file(const char *path, char **out, size_t *out_len) {
    *out = NULL;
    if (out_len) *out_len = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }
    long len = ftell(f);
    if (len < 0) {
        fclose(f);
        return -1;
    }
    rewind(f);
    char *buf = rn_xmalloc((size_t)len + 1);
    size_t readn = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[readn] = '\0';
    if (out_len) *out_len = readn;
    *out = buf;
    return 0;
}

static inline int rn_write_entire_file(const char *path, const char *data, size_t len) {
    if (rn_ensure_parent_dir(path) != 0) return -1;
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    if (len == (size_t)-1) len = data ? strlen(data) : 0;
    if (len > 0 && fwrite(data, 1, len, f) != len) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

static inline int rn_append_text(const char *path, const char *text) {
    if (rn_ensure_parent_dir(path) != 0) return -1;
    FILE *f = fopen(path, "ab");
    if (!f) return -1;
    size_t len = text ? strlen(text) : 0;
    if (len > 0 && fwrite(text, 1, len, f) != len) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

static inline int rn_copy_file(const char *src, const char *dst) {
    char *buf = NULL;
    size_t len = 0;
    if (rn_read_entire_file(src, &buf, &len) != 0) return -1;
    int rc = rn_write_entire_file(dst, buf, len);
    free(buf);
    return rc;
}

static inline int rn_now_iso8601(char *out, size_t out_sz) {
    time_t now = time(NULL);
    struct tm tmv;
    if (!localtime_r(&now, &tmv)) return -1;
    if (strftime(out, out_sz, "%Y-%m-%d %H:%M:%S", &tmv) == 0) return -1;
    return 0;
}

static inline long rn_file_size(FILE *f) {
    long cur = ftell(f);
    if (cur < 0) return -1;
    if (fseek(f, 0, SEEK_END) != 0) return -1;
    long end = ftell(f);
    if (end < 0) return -1;
    if (fseek(f, cur, SEEK_SET) != 0) return -1;
    return end;
}

static inline char *rn_basename_dup(const char *path) {
    const char *slash = strrchr(path, '/');
    return rn_xstrdup(slash ? slash + 1 : path);
}

static inline char *rn_dirname_dup(const char *path) {
    const char *slash = strrchr(path, '/');
    if (!slash) return rn_xstrdup(".");
    size_t len = (size_t)(slash - path);
    char *out = rn_xmalloc(len + 1);
    memcpy(out, path, len);
    out[len] = '\0';
    return out;
}

static inline bool rn_path_is_abs(const char *path) {
    return path && path[0] == '/';
}

static inline int rn_resolve_path(char *out, size_t out_sz, const char *base_dir, const char *path) {
    if (!path || !*path) {
        if (out_sz > 0) out[0] = '\0';
        return -1;
    }
    if (rn_path_is_abs(path)) {
        return snprintf(out, out_sz, "%s", path) >= (int)out_sz ? -1 : 0;
    }
    return snprintf(out, out_sz, "%s/%s", base_dir, path) >= (int)out_sz ? -1 : 0;
}

#endif
