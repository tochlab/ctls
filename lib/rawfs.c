#include "rawfs.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef enum {
    CMD_READ,
    CMD_WRITE
} cmd_direction_t;

typedef struct {
    void *buf;
    ssize_t len;
    ssize_t offset;
    cmd_direction_t dir;
} disk_cmd_t;

static void *rawdevice_write(rawdevice_t *dev, disk_cmd_t *arg);
static void *rawdevice_read(rawdevice_t *dev, disk_cmd_t *arg);
rawdevice_t *rawdevice_new(int fd, char *devname);
static void *rawdevice_loop(void *arg);
void rawdevice_free(rawdevice_t *dev);

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
    result->recordmap = calloc(prealloc, sizeof(record_t*) );
    result->recordmap_size = prealloc;
    result->thread_holder = calloc(devcount, sizeof(pthread_t) );
    
    return result;
}

void rawfs_free(rawfs_t *fs) {
    for( int i = 0;i<fs->device_count;i++) {
        rawdevice_free(fs->devices[i]);
        fs->devices[i] = NULL;
    }

    free(fs->devices);
    fs->devices = NULL;

    for( int i = 0;i<fs->recordmap_size;i++) {
        free(fs->recordmap[i]);
        fs->recordmap[i] = NULL;
    }

    free(fs->thread_holder);
    fs->thread_holder = NULL;

    free(fs);
    fs = NULL;
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
            newrecord->offset = -1;
            break;
        }
    }

    for( int i = 0;i<fs->device_count;i++ ) {
        if(newrecord->offset == -1) {
            newrecord->offset = fs->devices[i]->last_off;
            fs->devices[i]->last_off += newrecord->size;
        }
    }

    for( int i = 0;i<fs->device_count;i++ ) {
        disk_cmd_t *warg = calloc(1,sizeof(disk_cmd_t) );
        warg->buf = calloc(1, wlen);
        memcpy(warg->buf, &wbuf[wlen * i], wlen);
        warg->len = wlen;
        warg->dir = CMD_WRITE;
        warg->offset = newrecord->offset;
        //queue_push(fs->devices[i]->write_queue, warg);
        rawdevice_write(fs->devices[i], warg);
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
        disk_cmd_t *rarg = calloc(1,sizeof(disk_cmd_t));
        rarg->buf = buf_start;
        rarg->len = record_to_read->size;
        rarg->dir = CMD_READ;
        rarg->offset = record_to_read->offset;
        if(rarg->offset == -1) {
            fprintf(stderr,"\n!!!!\n");
            exit(1);
        }
        rawdevice_read(fs->devices[read_idx], rarg);
        buf_start+=record_to_read->size;
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

rawdevice_t *rawdevice_new(int fd, char *devname) {
    rawdevice_t *result = calloc(1, sizeof(rawdevice_t));
    result->fd = fd;
    result->devname = calloc(1, strlen(devname)+1);
    strcpy(result->devname, devname);
    result->last_off = 0;
    result->write_queue = queue_new();
    int thread_result = pthread_create(&result->thread, NULL, &rawdevice_loop, result);
    if( thread_result != 0) {
        fprintf(stderr,"Unable to start thread. Die...\n");
        exit(1);
    }
    return result;
}

void rawdevice_free(rawdevice_t *dev) {
    free(dev->devname);
    free(dev);
}

static void *rawdevice_write(rawdevice_t *device, disk_cmd_t *arg) {
    //fprintf(stderr,"WRITE: %s l %zu o %zu\n", device->devname, arg->len, arg->offset);
    if(lseek(device->fd, arg->offset,  SEEK_SET) < 0) {
        perror("lseek");
        fprintf(stderr,"offset: %zu\n", arg->offset);
        exit(1);
    };
    ssize_t wr = write(device->fd, arg->buf, arg->len);
    if(wr != arg->len) {
        fprintf(stderr,"rawdevice_write: Crash! %d %p %zu %zu\n", device->fd, arg->buf, arg->len, wr);
        exit(1);
    }
    free(arg->buf); // FIXME!
    free(arg);
    return NULL;
}

static void *rawdevice_read(rawdevice_t *device, disk_cmd_t *arg) {
    if( lseek( device->fd, arg->offset,SEEK_SET ) < 0) {
        perror("lseek");
        fprintf(stderr,"offset: %zu\n", arg->offset);
        exit(1);
    };
    ssize_t readed = read(device->fd, arg->buf, arg->len);
    if( readed != arg->len ) {
        fprintf(stderr,"rawdevice_read: ask %zu read %zu. Crash!\n", arg->len, readed);
        exit(1);
    }
    free(arg);
    return NULL;
}

static void *rawdevice_loop(void *arg) {
    rawdevice_t *device = (rawdevice_t *) arg;
    device->running = 1;
    while(device->running != 0) {
        if(device->write_queue->size != 0) {
            disk_cmd_t *cmd = queue_pop(device->write_queue);
            if(cmd != NULL) {
                rawdevice_write(device, cmd);
                sync();
            }
        }
    }
    return NULL;
}