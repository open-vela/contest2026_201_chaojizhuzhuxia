/****************************************************************************
 * apps/examples/yantu_watch/core/study_plan_manager.c
 *
 * 复习计划管理模块
 * - 每日任务清单（科目、内容、完成状态）
 * - JSON 持久化到 Flash
 * - 今日任务过滤
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cJSON.h>
#include "study_plan_manager.h"
#include "storage.h"
#include "json_parser.h"

#define PLAN_FILE "study_plan.json"

static plan_task_t g_tasks[PLAN_MAX_TASKS];
static int g_task_count = 0;
static int g_next_id = 1;

/* Save tasks to Flash */
static int save_tasks(void)
{
  cJSON *root = cJSON_CreateArray();
  if (!root) return -1;

  for (int i = 0; i < g_task_count; i++)
    {
      cJSON *item = cJSON_CreateObject();
      cJSON_AddNumberToObject(item, "id",      g_tasks[i].id);
      cJSON_AddStringToObject(item, "subject", g_tasks[i].subject);
      cJSON_AddStringToObject(item, "content", g_tasks[i].content);
      cJSON_AddBoolToObject(item,   "done",    g_tasks[i].done);
      cJSON_AddNumberToObject(item, "date",    g_tasks[i].date);
      cJSON_AddItemToArray(root, item);
    }

  int ret = json_save_file(PLAN_FILE, root);
  cJSON_Delete(root);
  return ret;
}

/* Load tasks from Flash */
static int load_tasks(void)
{
  cJSON *root = NULL;
  if (json_parse_file(PLAN_FILE, &root) != 0)
    return -1;

  if (!cJSON_IsArray(root))
    {
      cJSON_Delete(root);
      return -1;
    }

  g_task_count = cJSON_GetArraySize(root);
  if (g_task_count > PLAN_MAX_TASKS)
    g_task_count = PLAN_MAX_TASKS;

  g_next_id = 1;
  for (int i = 0; i < g_task_count; i++)
    {
      cJSON *item = cJSON_GetArrayItem(root, i);
      if (!item) continue;

      g_tasks[i].id   = json_get_int(item, "id", i + 1);
      g_tasks[i].done = json_get_bool(item, "done", false);
      g_tasks[i].date = json_get_int(item, "date", 20260701);

      const char *subject = json_get_string(item, "subject", "自习");
      strncpy(g_tasks[i].subject, subject, sizeof(g_tasks[i].subject) - 1);

      const char *content = json_get_string(item, "content", "");
      strncpy(g_tasks[i].content, content, sizeof(g_tasks[i].content) - 1);

      if (g_tasks[i].id >= g_next_id)
        g_next_id = g_tasks[i].id + 1;
    }

  cJSON_Delete(root);
  return 0;
}

int plan_init(void)
{
  g_task_count = 0;
  g_next_id = 1;

  if (load_tasks() != 0)
    {
      /* First run: add default tasks */
      plan_add_task("英语", "背单词 50 个");
      plan_add_task("数学", "做真题一套");
      plan_add_task("政治", "背诵知识点");
      plan_add_task("专业课", "复习笔记");
      save_tasks();
    }

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
  save_tasks();
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
          save_tasks();
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
          save_tasks();
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
  int max = *count;
  for (int i = 0; i < g_task_count && n < max; i++)
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