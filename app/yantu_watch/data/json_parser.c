/****************************************************************************
 * apps/examples/yantu_watch/data/json_parser.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "json_parser.h"

int json_parse_file(const char *path, cJSON **root)
{
  FILE *fp = fopen(path, "rb");
  if (!fp) return -1;

  fseek(fp, 0, SEEK_END);
  long len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *content = (char *)malloc(len + 1);
  if (!content)
    {
      fclose(fp);
      return -1;
    }

  fread(content, 1, len, fp);
  content[len] = '\0';
  fclose(fp);

  *root = cJSON_Parse(content);
  free(content);

  return (*root) ? 0 : -1;
}

int json_save_file(const char *path, cJSON *root)
{
  char *str = cJSON_Print(root);
  if (!str) return -1;

  FILE *fp = fopen(path, "w");
  if (!fp)
    {
      free(str);
      return -1;
    }

  fprintf(fp, "%s", str);
  fclose(fp);
  free(str);

  return 0;
}

int json_get_int(cJSON *obj, const char *key, int def)
{
  cJSON *item = cJSON_GetObjectItem(obj, key);
  if (item && cJSON_IsNumber(item))
    return item->valueint;
  return def;
}

bool json_get_bool(cJSON *obj, const char *key, bool def)
{
  cJSON *item = cJSON_GetObjectItem(obj, key);
  if (item && cJSON_IsBool(item))
    return item->valueint != 0;
  return def;
}

const char *json_get_string(cJSON *obj, const char *key, const char *def)
{
  cJSON *item = cJSON_GetObjectItem(obj, key);
  if (item && cJSON_IsString(item))
    return item->valuestring;
  return def;
}