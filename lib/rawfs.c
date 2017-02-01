#include "rawfs.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    void *buf;
    int len;
    record_t *rec;
    rawdevice_t *dev;
} disk_cmd_t;

rawfs_t *rawfs_new(char *devlist[], int devcount, int prealloc) {
    rawfs_t *result = (rawfs_t *) calloc(1, sizeof(rawfs_t));
    rawdevice_t **devices = (rawdevice_t **) calloc(devcount, sizeof(rawdevice_t*));

    for ( int idx = 0;idx < devcount;idx++) {
        fprintf(stderr,"Opening '%s' ...", devlist[idx]);
        int fd = open(devlist[idx], O_RDWR, O_SYNC);
        if(fd < 0) {
            perror(" ");
            fprintf(stderr,"FAIL\n");
            free(devices);
            free(result);
            return NULL;
        }
        rawdevice_t *device = rawdevice_new(fd, devlist[idx]);
        devices[idx] = device;
        fprintf(stderr, " OK(%d)\n", devices[idx]->fd);
    }
    result->devices = devices;
    result->device_count = devcount;
    result->recordmap = calloc(sizeof(record_t*), prealloc);
    result->recordmap_size = prealloc;
    result->thread_holder = calloc(sizeof(pthread_t), devcount);
    
    return result;
}

static void *rawdevice_write(void *arg) {
    disk_cmd_t *warg = (disk_cmd_t *) arg;

    if(lseek(warg->dev->fd, warg->dev->last_off,  SEEK_SET) < 0) {
        perror("lseek");
        exit(1);
    };
    int wr = write(warg->dev->fd, warg->buf, warg->len);
    warg->rec->size = wr;
    warg->rec->offset = warg->dev->last_off;
    warg->dev->last_off += wr;
    return NULL;
}

static void *rawdevice_read(void *arg) {
    disk_cmd_t *cmd = (disk_cmd_t *) arg;
    if( lseek( cmd->dev->fd, cmd->rec->offset,SEEK_SET ) < 0) {
        perror("lseek");
        exit(1);
    };
    int readed = read(cmd->dev->fd, cmd->buf, cmd->len);
    if( readed != cmd->len ) {
        fprintf(stderr,"rawdevice_read: ask %d read %d\n", cmd->len, readed);
    }
    return NULL;
}

int rawfs_write(rawfs_t *fs, void *buf, size_t size) {
    char *wbuf = (char *) buf;
    size_t wlen = size / fs->device_count;
    record_t *newrecord = calloc(1, sizeof(record_t));
    newrecord->flags = REC_ACTIVE;
    newrecord->size = wlen;
    int widx = -1;

    for(int i = 0;i<fs->recordmap_size;i++) {
        record_t *rec = fs->recordmap[i];
        if( rec != NULL) {
            if(rec->flags != REC_ACTIVE && rec->size == wlen) {
                widx = i;
                break;
            }
        }
        else {
            widx = i;
            fs->recordmap[i] = newrecord;
            break;
        }
    }

    for( int i = 0;i<fs->device_count;i++ ) {
        disk_cmd_t *warg = calloc(sizeof(disk_cmd_t), 1);
        warg->buf = &wbuf[wlen * i];
        warg->len = wlen;
        warg->rec = fs->recordmap[widx];
        warg->dev = fs->devices[i];
        pthread_create(&fs->thread_holder[i], NULL, &rawdevice_write, warg);
    }

    for(int i = 0;i<fs->device_count;i++) {
        pthread_join(fs->thread_holder[i], NULL);
    }

    return widx;
}

size_t rawfs_read(rawfs_t *fs, uint32_t id, void *buf, size_t size) {
    record_t *record_to_read = fs->recordmap[id];

    if(record_to_read == NULL) {
        return -1;
    }

    char *buf_start = buf;
    for(int read_idx=0;read_idx<fs->device_count;read_idx++) {
        disk_cmd_t *rarg = calloc(sizeof(disk_cmd_t), 1);
        rarg->buf = buf_start;
        rarg->len = record_to_read->size;
        rarg->rec = record_to_read;
        rarg->dev = fs->devices[read_idx];
        pthread_create(&fs->thread_holder[read_idx], NULL, &rawdevice_read,rarg);
        buf_start+=record_to_read->size;
    }

    for(int i = 0;i<fs->device_count;i++) {
        pthread_join(fs->thread_holder[i], NULL);
    }

    return size;
}

int rawfs_delete(rawfs_t *fs, uint32_t id) {
    record_t *rec = fs->recordmap[id];
    if( rec == NULL ) {
        return -1;
    }
    rec->flags = REC_DELETED;
    return 0;
}

rawdevice_t *rawdevice_new(int fd, char *devname) {
    rawdevice_t *result = calloc(1, sizeof(rawdevice_t));
    result->fd = fd;
    result->devname = calloc(1, strlen(devname)+1);
    strcpy(result->devname, devname);
    result->last_off = 0;
    return result;
}

void rawdevice_free(rawdevice_t *dev) {
    free(dev->devname);
    free(dev);
}

void rawfs_dump_maps(rawfs_t *fs) {
    fprintf(stderr,"Dumping records.\n");
    for(int i = 0;i<fs->device_count;i++) {
        fprintf(stderr,"%s:\n", fs->devices[i]->devname);
        record_t **rec_map = fs->recordmap;
        for(int p = 0;p<fs->recordmap_size;p++) {
            if( rec_map[p] != NULL ) {
                fprintf(stderr,"%4d %08zx %08zx %08x\n", p, rec_map[p]->offset, rec_map[p]->size, rec_map[p]->flags);
            }
        }
        fprintf(stderr,"Device last_off = %zd\n", fs->devices[i]->last_off);
    }
}
