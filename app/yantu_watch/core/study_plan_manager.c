/****************************************************************************
 * apps/examples/yantu_watch/core/study_plan_manager.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "study_plan_manager.h"

static plan_task_t g_tasks[PLAN_MAX_TASKS];
static int g_task_count = 0;
static int g_next_id = 1;

int plan_init(void)
{
  g_task_count = 0;
  g_next_id = 1;
  return 0;
}

int plan_add_task(const char *subject, const char *content)
{
  if (g_task_count >= PLAN_MAX_TASKS) return -1;

  plan_task_t *task = &g_tasks[g_task_count];
  task->id = g_next_id++;
  strncpy(task->subject, subject, sizeof(task->subject) - 1);
  strncpy(task->content, content, sizeof(task->content) - 1);
  task->done = false;

  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  task->date = (tm->tm_year + 1900) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;

  g_task_count++;
  return task->id;
}

int plan_remove_task(int id)
{
  for (int i = 0; i < g_task_count; i++)
    {
      if (g_tasks[i].id == id)
        {
          memmove(&g_tasks[i], &g_tasks[i + 1],
                  (g_task_count - i - 1) * sizeof(plan_task_t));
          g_task_count--;
          return 0;
        }
    }
  return -1;
}

int plan_set_done(int id, bool done)
{
  for (int i = 0; i < g_task_count; i++)
    {
      if (g_tasks[i].id == id)
        {
          g_tasks[i].done = done;
          return 0;
        }
    }
  return -1;
}

int plan_get_today_tasks(plan_task_t *tasks, int *count)
{
  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  int today = (tm->tm_year + 1900) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;

  int n = 0;
  for (int i = 0; i < g_task_count && n < *count; i++)
    {
      if (g_tasks[i].date == today)
        {
          tasks[n++] = g_tasks[i];
        }
    }

  *count = n;
  return 0;
}

int plan_get_task_count(void)
{
  return g_task_count;
}

int plan_get_task(int index, plan_task_t *task)
{
  if (index < 0 || index >= g_task_count) return -1;
  *task = g_tasks[index];
  return 0;
}