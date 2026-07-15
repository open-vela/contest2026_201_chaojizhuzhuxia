/****************************************************************************
 * apps/examples/yantu_watch/data/storage.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "storage.h"

#define STORAGE_BASE_PATH "/data"

int storage_init(void)
{
  /* Create data directory if not exists */
  mkdir(STORAGE_BASE_PATH, 0777);
  return 0;
}

int storage_write(const char *path, const void *data, size_t len)
{
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "%s/%s", STORAGE_BASE_PATH, path);

  FILE *fp = fopen(full_path, "wb");
  if (!fp) return -1;

  size_t written = fwrite(data, 1, len, fp);
  fclose(fp);

  return (written == len) ? 0 : -1;
}

int storage_read(const char *path, void *buf, size_t *len)
{
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "%s/%s", STORAGE_BASE_PATH, path);

  FILE *fp = fopen(full_path, "rb");
  if (!fp) return -1;

  *len = fread(buf, 1, *len, fp);
  fclose(fp);

  return 0;
}

int storage_delete(const char *path)
{
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "%s/%s", STORAGE_BASE_PATH, path);
  return remove(full_path);
}

bool storage_exists(const char *path)
{
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "%s/%s", STORAGE_BASE_PATH, path);

  struct stat st;
  return (stat(full_path, &st) == 0);
}

int storage_list(const char *dir, char (*files)[64], int *count)
{
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "%s/%s", STORAGE_BASE_PATH, dir);

  DIR *dp = opendir(full_path);
  if (!dp) return -1;

  int n = 0;
  struct dirent *entry;
  while ((entry = readdir(dp)) != NULL && n < *count)
    {
      if (entry->d_name[0] != '.')
        {
          strncpy(files[n], entry->d_name, 63);
          n++;
        }
    }

  closedir(dp);
  *count = n;
  return 0;
}