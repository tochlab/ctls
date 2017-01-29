#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

typedef enum {
    REC_ACTIVE,
    REC_DELETED
} record_state;

typedef struct {
    uint32_t key;
    off_t offset;
    size_t size;
    record_state flags;
} record_t;

typedef struct {
    int fd;
    char *devname;
    record_t **recordmap;
    size_t recordmap_size;
    size_t last_off;
} rawdevice_t;

typedef struct {
    rawdevice_t **devices;
    int device_count;
} rawfs_t;

rawfs_t *rawfs_new(char *devlist[], int devcount);
int rawfs_delete(rawfs_t *fs, uint32_t id);
size_t rawfs_write(rawfs_t *fs, uint32_t id, void *buf, size_t size);
size_t rawfs_read(rawfs_t *fs, uint32_t id, void *buf, size_t size);

rawdevice_t *rawdevice_new(int fd, char *devname);
void rawdevice_addrecord(rawdevice_t *device, record_t *rec);

void rawfs_dump_maps(rawfs_t *fs);