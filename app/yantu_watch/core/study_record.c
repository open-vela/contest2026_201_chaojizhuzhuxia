/****************************************************************************
 * apps/examples/yantu_watch/core/study_record.c
 *
 * 学习时长统计模块
 * - 每日学习时长记录，精度到分钟
 * - 按日持久化到 Flash（JSON 数组）
 * - 支持周/月统计（最近 7 天 / 30 天）
 ****************************************************************************/

#include <nuttx/config.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "study_record.h"
#include "storage.h"
#include "json_parser.h"
#include <cJSON.h>

#define MAX_DAYS 365
#define RECORD_FILE "study_record.json"

/* 每日记录结构 */
typedef struct
{
  int date;     /* YYYYMMDD */
  int minutes;  /* 当日学习分钟数 */
} day_record_t;

/* 缓存 */
static day_record_t g_days[MAX_DAYS];
static int g_day_count = 0;
static int g_today_minutes = 0;
static int g_total_minutes = 0;
static int g_total_days   = 0;

/* 获取今天日期 YYYYMMDD */
static int get_today_date(void)
{
  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  return (tm->tm_year + 1900) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;
}

/* 在缓存中查找指定日期的索引，返回 -1 表示未找到 */
static int find_day_index(int date)
{
  for (int i = 0; i < g_day_count; i++)
    {
      if (g_days[i].date == date)
        return i;
    }
  return -1;
}

/* 计算缓存中的汇总统计 */
static void recalc_summary(void)
{
  g_today_minutes = 0;
  g_total_minutes = 0;
  g_total_days    = 0;

  int today = get_today_date();

  for (int i = 0; i < g_day_count; i++)
    {
      g_total_minutes += g_days[i].minutes;
      if (g_days[i].minutes > 0)
        g_total_days++;

      if (g_days[i].date == today)
        g_today_minutes = g_days[i].minutes;
    }
}

/* 从 Flash 加载记录 */
static int load_record(void)
{
  cJSON *root = NULL;
  if (json_parse_file(RECORD_FILE, &root) != 0)
    return -1;

  cJSON *days = cJSON_GetObjectItem(root, "days");
  if (!days || !cJSON_IsArray(days))
    {
      cJSON_Delete(root);
      return -1;
    }

  g_day_count = cJSON_GetArraySize(days);
  if (g_day_count > MAX_DAYS)
    g_day_count = MAX_DAYS;

  for (int i = 0; i < g_day_count; i++)
    {
      cJSON *item = cJSON_GetArrayItem(days, i);
      if (!item) continue;
      g_days[i].date    = json_get_int(item, "date", 0);
      g_days[i].minutes = json_get_int(item, "minutes", 0);
    }

  cJSON_Delete(root);

  recalc_summary();
  return 0;
}

/* 保存记录到 Flash */
static int save_record(void)
{
  cJSON *root = cJSON_CreateObject();
  if (!root) return -1;

  cJSON *days = cJSON_AddArrayToObject(root, "days");

  for (int i = 0; i < g_day_count; i++)
    {
      cJSON *item = cJSON_CreateObject();
      cJSON_AddNumberToObject(item, "date",    g_days[i].date);
      cJSON_AddNumberToObject(item, "minutes", g_days[i].minutes);
      cJSON_AddItemToArray(days, item);
    }

  int ret = json_save_file(RECORD_FILE, root);
  cJSON_Delete(root);
  return ret;
}

int study_record_init(void)
{
  g_day_count = 0;
  g_today_minutes = 0;
  g_total_minutes = 0;
  g_total_days = 0;

  if (load_record() != 0)
    {
      /* First run: create empty record */
      save_record();
    }

  return 0;
}

int study_record_add_minutes(int minutes)
{
  int today = get_today_date();
  int idx = find_day_index(today);

  if (idx < 0)
    {
      /* First record for today */
      if (g_day_count >= MAX_DAYS)
        {
          /* Shift oldest entry out */
          memmove(&g_days[0], &g_days[1], (MAX_DAYS - 1) * sizeof(day_record_t));
          g_day_count = MAX_DAYS - 1;
        }

      idx = g_day_count;
      g_days[idx].date = today;
      g_days[idx].minutes = 0;
      g_day_count++;
    }

  g_days[idx].minutes += minutes;

  recalc_summary();
  return save_record();
}

int study_record_get_today_minutes(void)
{
  return g_today_minutes;
}

int study_record_get_week_data(int *data, int *count)
{
  int today = get_today_date();
  int max_days = (*count < 7) ? *count : 7;
  *count = max_days;

  memset(data, 0, max_days * sizeof(int));

  for (int i = 0; i < max_days; i++)
    {
      /* day_offset: 0 = today, 1 = yesterday, ... */
      int target_date = today - i;
      int idx = find_day_index(target_date);
      if (idx >= 0)
        data[max_days - 1 - i] = g_days[idx].minutes;
    }

  return 0;
}

int study_record_get_month_data(int *data, int *count)
{
  int today = get_today_date();
  int max_days = (*count < 30) ? *count : 30;
  *count = max_days;

  memset(data, 0, max_days * sizeof(int));

  for (int i = 0; i < max_days; i++)
    {
      int target_date = today - i;
      int idx = find_day_index(target_date);
      if (idx >= 0)
        data[max_days - 1 - i] = g_days[idx].minutes;
    }

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