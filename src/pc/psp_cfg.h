#ifdef TARGET_PSP
#include <PR/ultratypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pspkernel.h>
#include <pspiofilemgr.h>

#define PSP_CFG_DIR "ms0:/PSP/GAME/mario64/"

typedef struct {
    char *buf;
    size_t len, cap, pos;
    int fd;
    int writing;
} PSPCFG;

static PSPCFG *psp_cfg_fopen(const char *name, const char *mode) {
    char path[256];
    PSPCFG *f;
    int fd;
    int flags = (mode[0] == 'w') ? (PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC) : PSP_O_RDONLY;

    snprintf(path, sizeof(path), "%s%s", PSP_CFG_DIR, name);
    fd = sceIoOpen(path, flags, 0777);
    if (fd < 0) return NULL;
    f = (PSPCFG *) calloc(1, sizeof(PSPCFG));
    if (f == NULL) { sceIoClose(fd); return NULL; }
    f->fd = fd;
    f->writing = (mode[0] == 'w');
    if (!f->writing) {
        int size = (int) sceIoLseek(fd, 0, PSP_SEEK_END);
        sceIoLseek(fd, 0, PSP_SEEK_SET);
        if (size > 0) {
            f->buf = (char *) malloc(size);
            if (f->buf != NULL) {
                int n = sceIoRead(fd, f->buf, size);
                f->len = (n > 0) ? (size_t) n : 0;
            }
        }
        sceIoClose(fd);
        f->fd = -1;
    }
    return f;
}

static char *psp_cfg_fgets(char *s, int size, PSPCFG *f) {
    int i = 0;
    if (f->pos >= f->len || size <= 1) return NULL;
    while (i < size - 1 && f->pos < f->len) {
        char c = f->buf[f->pos++];
        s[i++] = c;
        if (c == '\n') break;
    }
    s[i] = '\0';
    return s;
}

static int psp_cfg_feof(PSPCFG *f) {
    return f->pos >= f->len;
}

static int psp_cfg_fprintf(PSPCFG *f, const char *fmt, ...) {
    char tmp[256];
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsnprintf(tmp, sizeof(tmp), fmt, ap);
    va_end(ap);
    if (n < 0) return n;
    if (n >= (int) sizeof(tmp)) n = (int) sizeof(tmp) - 1;
    if (f->len + n > f->cap) {
        size_t newcap = f->cap ? f->cap * 2 : 1024;
        char *nb;
        while (newcap < f->len + n) newcap *= 2;
        nb = (char *) realloc(f->buf, newcap);
        if (nb == NULL) return -1;
        f->buf = nb;
        f->cap = newcap;
    }
    memcpy(f->buf + f->len, tmp, n);
    f->len += n;
    return n;
}

static int psp_cfg_fclose(PSPCFG *f) {
    if (f->writing && f->fd >= 0) {
        if (f->len > 0) sceIoWrite(f->fd, f->buf, (int) f->len);
        sceIoClose(f->fd);
    }
    free(f->buf);
    free(f);
    return 0;
}

#undef FILE
#undef fopen
#undef fgets
#undef feof
#undef fprintf
#undef fclose
#define FILE PSPCFG
#define fopen psp_cfg_fopen
#define fgets psp_cfg_fgets
#define feof psp_cfg_feof
#define fprintf psp_cfg_fprintf
#define fclose psp_cfg_fclose
#endif
