/****************************************************************************
 * apps/examples/yantu_watch/data/database.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <cJSON.h>
#include "database.h"
#include "storage.h"
#include "json_parser.h"

static char g_db_path[] = "settings.json";

int db_init(void)
{
  return 0;
}

int db_set_str(const char *key, const char *value)
{
  cJSON *root = NULL;
  if (storage_exists(g_db_path))
    {
      json_parse_file(g_db_path, &root);
    }

  if (!root)
    root = cJSON_CreateObject();

  cJSON_AddStringToObject(root, key, value);
  int ret = json_save_file(g_db_path, root);
  cJSON_Delete(root);
  return ret;
}

int db_get_str(const char *key, char *value, size_t maxlen)
{
  cJSON *root = NULL;
  if (!storage_exists(g_db_path)) return -1;

  if (json_parse_file(g_db_path, &root) != 0) return -1;

  const char *val = json_get_string(root, key, NULL);
  if (val)
    {
      strncpy(value, val, maxlen - 1);
      value[maxlen - 1] = '\0';
      cJSON_Delete(root);
      return 0;
    }

  cJSON_Delete(root);
  return -1;
}

int db_set_int(const char *key, int value)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", value);
  return db_set_str(key, buf);
}

int db_get_int(const char *key, int def)
{
  char buf[32];
  if (db_get_str(key, buf, sizeof(buf)) == 0)
    return atoi(buf);
  return def;
}

int db_delete(const char *key)
{
  cJSON *root = NULL;
  if (!storage_exists(g_db_path)) return 0;

  if (json_parse_file(g_db_path, &root) != 0) return -1;

  cJSON_DeleteItemFromObject(root, key);
  int ret = json_save_file(g_db_path, root);
  cJSON_Delete(root);
  return ret;
}