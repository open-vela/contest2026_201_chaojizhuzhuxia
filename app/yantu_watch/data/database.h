#ifndef __YANTU_DATA_DATABASE_H
#define __YANTU_DATA_DATABASE_H

#include <stdbool.h>

/* Lightweight database interface (using UnQLite or JSON) */

int  db_init(void);
int  db_set_str(const char *key, const char *value);
int  db_get_str(const char *key, char *value, size_t maxlen);
int  db_set_int(const char *key, int value);
int  db_get_int(const char *key, int def);
int  db_delete(const char *key);

#endif /* __YANTU_DATA_DATABASE_H */