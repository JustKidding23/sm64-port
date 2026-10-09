#include <stdio.h>
#ifdef TARGET_PSP
#include <pspkernel.h>
#include <pspiofilemgr.h>

#define PSP_SAVE_PATH "ms0:/PSP/GAME/mario64/sm64_save_file.bin"

typedef struct { int fd; } PSPFILE;
static PSPFILE psp_save_file;

static PSPFILE *psp_fopen(const char *name, const char *mode) {
    int flags = (mode[0] == 'w') ? (PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC) : PSP_O_RDONLY;
    int fd = sceIoOpen(PSP_SAVE_PATH, flags, 0777);
    (void) name;
    if (fd < 0) return NULL;
    psp_save_file.fd = fd;
    return &psp_save_file;
}

static int psp_fread(void *buf, int size, int count, PSPFILE *fp) {
    int n = sceIoRead(fp->fd, buf, size * count);
    return n < 0 ? 0 : n / size;
}

static int psp_fwrite(const void *buf, int size, int count, PSPFILE *fp) {
    int n = sceIoWrite(fp->fd, buf, size * count);
    return n < 0 ? 0 : n / size;
}

static int psp_fclose(PSPFILE *fp) {
    sceIoClose(fp->fd);
    return 0;
}
#else
#define PSPFILE FILE
#define psp_fopen fopen
#define psp_fread fread
#define psp_fwrite fwrite
#define psp_fclose fclose
#endif
