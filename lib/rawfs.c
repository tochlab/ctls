#include "rawfs.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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
        rawdevice_t *device = rawdevice_new(fd, devlist[idx], prealloc);
        devices[idx] = device;
        fprintf(stderr, " OK(%d)\n", devices[idx]->fd);
    }
    result->devices = devices;
    result->device_count = devcount;
    
    return result;
}

int rawfs_write(rawfs_t *fs, void *buf, size_t size) {
    char *wbuf = (char *) buf;
    size_t wlen = size / fs->device_count;
    int result = -1;

    for( int i = 0;i<fs->device_count;i++ ) {
        lseek(fs->devices[i]->fd, SEEK_SET, fs->devices[i]->last_off);
        int wr = write(fs->devices[i]->fd, &wbuf[wlen * i], wlen);
        record_t *rec = calloc(1, sizeof(record_t));
        rec->flags = REC_ACTIVE;
        rec->size = wr;
        rec->offset = fs->devices[i]->last_off;
        fs->devices[i]->last_off += wr;
        result = rawdevice_addrecord(fs->devices[i], rec);
    }
    return result;
}

size_t rawfs_read(rawfs_t *fs, uint32_t id, void *buf, size_t size) {
    record_t **records_to_read = calloc(fs->device_count, sizeof(record_t*));

    for(int dev_idx=0;dev_idx<fs->device_count;dev_idx++) {
            records_to_read[dev_idx] = fs->devices[dev_idx]->recordmap[id];
    }

    char *buf_start = buf;
    size_t readed = 0;
    for(int read_idx=0;read_idx<fs->device_count;read_idx++) {
        lseek(fs->devices[read_idx]->fd, SEEK_SET, records_to_read[read_idx]->offset);
        readed += read(fs->devices[read_idx]->fd, buf_start, records_to_read[read_idx]->size);
        buf_start+=records_to_read[read_idx]->size;
    }

    if( size != readed ) {
        fprintf(stderr,"rawfs_read warning: size %zd readed %zd", size, readed);
    }

    return 0;
}

int rawfs_delete(rawfs_t *fs, uint32_t id) {
    for(int dev_idx = 0;dev_idx<fs->device_count;dev_idx++) {
        record_t *rec = fs->devices[dev_idx]->recordmap[id];
        if( rec != NULL) {
                rec->flags = REC_DELETED;
        }
    }
    return 0;
}

rawdevice_t *rawdevice_new(int fd, char *devname, int prealloc) {
    rawdevice_t *result = calloc(1, sizeof(rawdevice_t));
    result->fd = fd;
    result->devname = calloc(1, strlen(devname)+1);
    strcpy(result->devname, devname);
    result->last_off = 0;
    result->recordmap = calloc(prealloc, sizeof(record_t*));
    result->recordmap_size = prealloc;
    return result;
}

int rawdevice_addrecord(rawdevice_t *device, record_t *newrec) {
    record_t **map = device->recordmap;
    for(int i = 0;i<device->recordmap_size-1;i++) {
        record_t *rec = map[i];
        if( rec == NULL) {
            map[i] = newrec;
            return i;
        }

        if( rec->flags != REC_ACTIVE && rec->size == newrec->size) {
            free(rec);
            map[i] = newrec;
            return i;
        }
    }
    return -1;
}

void rawdevice_free(rawdevice_t *dev) {
    free(dev->devname);
    free(dev);
}

void rawfs_dump_maps(rawfs_t *fs) {
    fprintf(stderr,"Dumping records.\n");
    for(int i = 0;i<fs->device_count;i++) {
        fprintf(stderr,"%s:\n", fs->devices[i]->devname);
        record_t **rec_map = fs->devices[i]->recordmap;
        for(int p = 0;p<fs->devices[i]->recordmap_size;p++) {
            if( rec_map[p] != NULL ) {
                fprintf(stderr,"%4d %08zx %08zx %08x\n", p, rec_map[p]->offset, rec_map[p]->size, rec_map[p]->flags);
            }
        }
        fprintf(stderr,"Device last_off = %zd\n", fs->devices[i]->last_off);
    }
}
