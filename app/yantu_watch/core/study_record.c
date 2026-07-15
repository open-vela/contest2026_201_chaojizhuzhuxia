/****************************************************************************
 * apps/examples/yantu_watch/core/study_record.c
 *
 * 学习时长统计模块
 * - 每日学习时长记录，精度到分钟
 * - 持久化到 Flash，支持周/月统计
 ****************************************************************************/

#include <nuttx/config.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "study_record.h"
#include "storage.h"
#include "json_parser.h"
#include <cJSON.h>

#define MAX_DAYS 365

#define RECORD_FILE "study_record.json"

static int g_today_minutes = 0;
static int g_total_minutes = 0;
static int g_total_days   = 0;

/* Load record from Flash */
static int load_record(void)
{
  cJSON *root = NULL;
  if (json_parse_file(RECORD_FILE, &root) != 0)
    return -1;

  g_today_minutes = json_get_int(root, "today_minutes", 0);
  g_total_minutes = json_get_int(root, "total_minutes", 0);
  g_total_days    = json_get_int(root, "total_days", 0);

  cJSON_Delete(root);
  return 0;
}

/* Save record to Flash */
static int save_record(void)
{
  cJSON *root = cJSON_CreateObject();
  if (!root) return -1;

  cJSON_AddNumberToObject(root, "today_minutes", g_today_minutes);
  cJSON_AddNumberToObject(root, "total_minutes", g_total_minutes);
  cJSON_AddNumberToObject(root, "total_days",    g_total_days);

  int ret = json_save_file(RECORD_FILE, root);
  cJSON_Delete(root);
  return ret;
}

int study_record_init(void)
{
  /* Try to load existing record */
  if (load_record() != 0)
    {
      /* First run: create default record */
      g_today_minutes = 0;
      g_total_minutes = 0;
      g_total_days    = 0;
      save_record();
    }
  return 0;
}

int study_record_add_minutes(int minutes)
{
  g_today_minutes += minutes;
  g_total_minutes += minutes;

  /* If this is the first study of the day, increment day count */
  if (g_today_minutes <= minutes)
    g_total_days++;

  return save_record();
}

int study_record_get_today_minutes(void)
{
  return g_today_minutes;
}

int study_record_get_week_data(int *data, int *count)
{
  /* TODO: Load detailed daily records from week log */
  *count = 7;
  memset(data, 0, 7 * sizeof(int));
  data[6] = g_today_minutes; /* Today is the last day of the week view */
  return 0;
}

int study_record_get_month_data(int *data, int *count)
{
  *count = 30;
  memset(data, 0, 30 * sizeof(int));
  data[29] = g_today_minutes;
  return 0;
}

int study_record_get_total_days(void)
{
  return g_total_days;
}

int study_record_get_total_minutes(void)
{
  return g_total_minutes;
}