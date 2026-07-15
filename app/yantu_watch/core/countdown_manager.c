/****************************************************************************
 * apps/examples/yantu_watch/core/countdown_manager.c
 *
 * 考研倒计时管理模块
 * - 基于 RTC 实时计算距离考研日期的时间
 * - 支持自定义考试日期，持久化到 Flash
 ****************************************************************************/

#include <nuttx/config.h>
#include <time.h>
#include "countdown_manager.h"
#include "database.h"

#define KEY_EXAM_YEAR  "exam_year"
#define KEY_EXAM_MONTH "exam_month"
#define KEY_EXAM_DAY   "exam_day"

/* Default exam date: 2026-12-19 (全国硕士研究生招生考试) */
#define DEFAULT_YEAR  2026
#define DEFAULT_MONTH 12
#define DEFAULT_DAY   19

static int g_exam_year;
static int g_exam_month;
static int g_exam_day;

int countdown_init(void)
{
  /* Load saved exam date from Flash, fallback to default */
  g_exam_year = db_get_int(KEY_EXAM_YEAR, DEFAULT_YEAR);
  g_exam_month = db_get_int(KEY_EXAM_MONTH, DEFAULT_MONTH);
  g_exam_day = db_get_int(KEY_EXAM_DAY, DEFAULT_DAY);
  return 0;
}

int countdown_get_days_remaining(void)
{
  time_t now;
  struct tm exam_tm = { 0 };
  time(&now);

  exam_tm.tm_year = g_exam_year - 1900;
  exam_tm.tm_mon  = g_exam_month - 1;
  exam_tm.tm_mday = g_exam_day;
  exam_tm.tm_hour = 0;
  exam_tm.tm_min  = 0;
  exam_tm.tm_sec  = 0;

  time_t exam_time = mktime(&exam_tm);
  double diff = difftime(exam_time, now);
  return (diff > 0) ? (int)(diff / 86400) : 0;
}

void countdown_get_time_remaining(int *days, int *hours, int *mins, int *secs)
{
  time_t now;
  struct tm exam_tm = { 0 };
  time(&now);

  exam_tm.tm_year = g_exam_year - 1900;
  exam_tm.tm_mon  = g_exam_month - 1;
  exam_tm.tm_mday = g_exam_day;
  exam_tm.tm_hour = 0;
  exam_tm.tm_min  = 0;
  exam_tm.tm_sec  = 0;

  time_t exam_time = mktime(&exam_tm);
  double diff = difftime(exam_time, now);
  if (diff > 0)
    {
      *days  = (int)(diff / 86400);
      *hours = (int)((diff - *days * 86400) / 3600);
      *mins  = (int)((diff - *days * 86400 - *hours * 3600) / 60);
      *secs  = (int)(diff - *days * 86400 - *hours * 3600 - *mins * 60);
    }
  else
    {
      *days  = 0;
      *hours = 0;
      *mins  = 0;
      *secs  = 0;
    }
}

void countdown_set_exam_date(int year, int month, int day)
{
  g_exam_year  = year;
  g_exam_month = month;
  g_exam_day   = day;

  /* Persist to Flash */
  db_set_int(KEY_EXAM_YEAR, year);
  db_set_int(KEY_EXAM_MONTH, month);
  db_set_int(KEY_EXAM_DAY, day);
}

void countdown_get_exam_date(int *year, int *month, int *day)
{
  *year  = g_exam_year;
  *month = g_exam_month;
  *day   = g_exam_day;
}