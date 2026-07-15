#ifndef __YANTU_DATA_STORAGE_H
#define __YANTU_DATA_STORAGE_H

#include <stdbool.h>
#include <stddef.h>

int  storage_init(void);
int  storage_write(const char *path, const void *data, size_t len);
int  storage_read(const char *path, void *buf, size_t *len);
int  storage_delete(const char *path);
bool storage_exists(const char *path);
int  storage_list(const char *dir, char (*files)[64], int *count);

#endif /* __YANTU_DATA_STORAGE_H */