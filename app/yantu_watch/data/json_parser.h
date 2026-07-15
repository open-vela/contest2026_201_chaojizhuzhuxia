#ifndef __YANTU_DATA_JSON_PARSER_H
#define __YANTU_DATA_JSON_PARSER_H

#include <stdbool.h>

/* Forward declaration for cJSON */
typedef struct cJSON cJSON;

int   json_parse_file(const char *path, cJSON **root);
int   json_save_file(const char *path, cJSON *root);
int   json_get_int(cJSON *obj, const char *key, int def);
bool  json_get_bool(cJSON *obj, const char *key, bool def);
const char *json_get_string(cJSON *obj, const char *key, const char *def);

#endif /* __YANTU_DATA_JSON_PARSER_H */