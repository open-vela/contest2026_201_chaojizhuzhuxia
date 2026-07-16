/****************************************************************************
 * apps/examples/yantu_watch/core/reminder_manager.c
 *
 * 定时震动提醒模块
 * - 多组闹钟管理（增删改查）
 * - 支持重复（每天/仅一次）
 * - JSON 持久化到 Flash
 * - 基于 RTC 的闹钟检查
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cJSON.h>
#include "reminder_manager.h"
#include "storage.h"
#include "json_parser.h"
#include "vibrator.h"

#define REMINDER_FILE "reminders.json"

static reminder_item_t g_reminders[REMINDER_MAX_ITEMS];
static int g_reminder_count = 0;
static int g_next_id = 1;
static int g_last_checked_min = -1; /* Prevent repeated triggers */

/* Save reminders to Flash */
static int save_reminders(void)
{
  cJSON *root = cJSON_CreateArray();
  if (!root) return -1;

  for (int i = 0; i < g_reminder_count; i++)
    {
      cJSON *item = cJSON_CreateObject();
      cJSON_AddNumberToObject(item, "id",      g_reminders[i].id);
      cJSON_AddStringToObject(item, "label",   g_reminders[i].label);
      cJSON_AddNumberToObject(item, "hour",    g_reminders[i].hour);
      cJSON_AddNumberToObject(item, "minute",  g_reminders[i].minute);
      cJSON_AddBoolToObject(item,   "repeat",  g_reminders[i].repeat);
      cJSON_AddBoolToObject(item,   "enabled", g_reminders[i].enabled);
      cJSON_AddBoolToObject(item,   "vibrate", g_reminders[i].vibrate);
      cJSON_AddItemToArray(root, item);
    }

  int ret = json_save_file(REMINDER_FILE, root);
  cJSON_Delete(root);
  return ret;
}

/* Load reminders from Flash */
static int load_reminders(void)
{
  cJSON *root = NULL;
  if (json_parse_file(REMINDER_FILE, &root) != 0)
    return -1;

  if (!cJSON_IsArray(root))
    {
      cJSON_Delete(root);
      return -1;
    }

  g_reminder_count = cJSON_GetArraySize(root);
  if (g_reminder_count > REMINDER_MAX_ITEMS)
    g_reminder_count = REMINDER_MAX_ITEMS;

  g_next_id = 1;
  for (int i = 0; i < g_reminder_count; i++)
    {
      cJSON *item = cJSON_GetArrayItem(root, i);
      if (!item) continue;

      g_reminders[i].id      = json_get_int(item, "id", i + 1);
      g_reminders[i].hour    = json_get_int(item, "hour", 8);
      g_reminders[i].minute  = json_get_int(item, "minute", 0);
      g_reminders[i].repeat  = json_get_bool(item, "repeat", true);
      g_reminders[i].enabled = json_get_bool(item, "enabled", true);
      g_reminders[i].vibrate = json_get_bool(item, "vibrate", true);

      const char *label = json_get_string(item, "label", "提醒");
      strncpy(g_reminders[i].label, label, sizeof(g_reminders[i].label) - 1);

      if (g_reminders[i].id >= g_next_id)
        g_next_id = g_reminders[i].id + 1;
    }

  cJSON_Delete(root);
  return 0;
}

int reminder_init(void)
{
  g_reminder_count = 0;
  g_next_id = 1;
  g_last_checked_min = -1;

  /* Try to load saved reminders */
  if (load_reminders() != 0)
    {
      /* First run: add default reminders */
      reminder_add("起床",     7, 0,  true);
      reminder_add("午休结束", 13, 30, true);
      reminder_add("晚复盘",   21, 0,  true);
      save_reminders();
    }

  return 0;
}

int reminder_add(const char *label, int hour, int minute, bool repeat)
{
  if (g_reminder_count >= REMINDER_MAX_ITEMS) return -1;

  reminder_item_t *item = &g_reminders[g_reminder_count];
  item->id = g_next_id++;
  strncpy(item->label, label, sizeof(item->label) - 1);
  item->hour = hour;
  item->minute = minute;
  item->repeat = repeat;
  item->enabled = true;
  item->vibrate = true;

  g_reminder_count++;
  save_reminders();
  return item->id;
}

int reminder_remove(int id)
{
  for (int i = 0; i < g_reminder_count; i++)
    {
      if (g_reminders[i].id == id)
        {
          memmove(&g_reminders[i], &g_reminders[i + 1],
                  (g_reminder_count - i - 1) * sizeof(reminder_item_t));
          g_reminder_count--;
          save_reminders();
          return 0;
        }
    }
  return -1;
}

int reminder_update(int id, const reminder_item_t *item)
{
  for (int i = 0; i < g_reminder_count; i++)
    {
      if (g_reminders[i].id == id)
        {
          g_reminders[i] = *item;
          save_reminders();
          return 0;
        }
    }
  return -1;
}

int reminder_get_count(void)
{
  return g_reminder_count;
}

int reminder_get_item(int index, reminder_item_t *item)
{
  if (index < 0 || index >= g_reminder_count) return -1;
  *item = g_reminders[index];
  return 0;
}

int reminder_get_item_by_id(int id, reminder_item_t *item)
{
  for (int i = 0; i < g_reminder_count; i++)
    {
      if (g_reminders[i].id == id)
        {
          *item = g_reminders[i];
          return 0;
        }
    }
  return -1;
}

int reminder_set_enabled(int id, bool enabled)
{
  for (int i = 0; i < g_reminder_count; i++)
    {
      if (g_reminders[i].id == id)
        {
          g_reminders[i].enabled = enabled;
          save_reminders();
          return 0;
        }
    }
  return -1;
}

void reminder_check_alarm(void)
{
  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  int current_min = tm->tm_hour * 60 + tm->tm_min;

  /* Only check once per minute */
  if (current_min == g_last_checked_min)
    return;
  g_last_checked_min = current_min;

  for (int i = 0; i < g_reminder_count; i++)
    {
      if (!g_reminders[i].enabled)
        continue;

      int alarm_min = g_reminders[i].hour * 60 + g_reminders[i].minute;
      if (alarm_min == current_min)
        {
          /* Alarm triggered! */
          printf("reminder: ⏰ %s (%02d:%02d)\n",
                 g_reminders[i].label,
                 g_reminders[i].hour,
                 g_reminders[i].minute);

          if (g_reminders[i].vibrate)
            vibrator_set(VIBRATE_DOUBLE);

          /* Non-repeating alarms get disabled after trigger */
          if (!g_reminders[i].repeat)
            {
              g_reminders[i].enabled = false;
              save_reminders();
            }
        }
    }
}