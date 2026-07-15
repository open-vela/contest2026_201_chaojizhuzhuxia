/****************************************************************************
 * apps/examples/yantu_watch/core/reminder_manager.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "reminder_manager.h"

static reminder_item_t g_reminders[REMINDER_MAX_ITEMS];
static int g_reminder_count = 0;
static int g_next_id = 1;

int reminder_init(void)
{
  g_reminder_count = 0;
  g_next_id = 1;
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

int reminder_set_enabled(int id, bool enabled)
{
  for (int i = 0; i < g_reminder_count; i++)
    {
      if (g_reminders[i].id == id)
        {
          g_reminders[i].enabled = enabled;
          return 0;
        }
    }
  return -1;
}

void reminder_check_alarm(void)
{
  /* TODO: Check if any alarm should trigger */
}