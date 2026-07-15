/****************************************************************************
 * apps/examples/yantu_watch/core/pomodoro_timer.c
 *
 * 番茄自习计时器模块
 * - 状态机：空闲 → 专注 → 休息 → 完成
 * - 支持多科目切换
 * - 完成自动记录学习时长，触发震动回调
 ****************************************************************************/

#include <nuttx/config.h>
#include <time.h>
#include <string.h>
#include "pomodoro_timer.h"
#include "study_record.h"

/* Subject display names */
static const char *g_subject_names[] = {
  "政治", "英语", "数学", "专业课", "自定义"
};

/* State */
static pomodoro_state_t g_state = POMODORO_IDLE;
static subject_t       g_subject  = SUBJECT_ENGLISH;
static int             g_focus_minutes = 25;
static int             g_break_minutes = 5;
static int             g_remaining_secs = 0;
static int             g_today_count = 0;
static time_t          g_last_tick = 0;
static pomodoro_callback_t g_callback = NULL;

int pomodoro_init(void)
{
  g_state = POMODORO_IDLE;
  g_today_count = 0;
  g_remaining_secs = 0;
  g_callback = NULL;
  return 0;
}

void pomodoro_register_callback(pomodoro_callback_t cb)
{
  g_callback = cb;
}

int pomodoro_start(subject_t subject)
{
  if (g_state != POMODORO_IDLE && g_state != POMODORO_COMPLETED)
    return -1;

  g_subject = subject;
  g_remaining_secs = g_focus_minutes * 60;
  g_state = POMODORO_FOCUS;
  g_last_tick = time(NULL);

  if (g_callback)
    g_callback(POMODORO_FOCUS, g_subject);

  return 0;
}

int pomodoro_pause(void)
{
  if (g_state != POMODORO_FOCUS) return -1;
  g_state = POMODORO_BREAK;
  if (g_callback)
    g_callback(POMODORO_BREAK, g_subject);
  return 0;
}

int pomodoro_resume(void)
{
  if (g_state != POMODORO_BREAK) return -1;

  /* If break time is over, restart a new focus session */
  if (g_remaining_secs <= 0)
    {
      g_remaining_secs = g_focus_minutes * 60;
    }

  g_state = POMODORO_FOCUS;
  g_last_tick = time(NULL);
  if (g_callback)
    g_callback(POMODORO_FOCUS, g_subject);
  return 0;
}

int pomodoro_stop(void)
{
  g_state = POMODORO_IDLE;
  g_remaining_secs = 0;
  if (g_callback)
    g_callback(POMODORO_IDLE, g_subject);
  return 0;
}

int pomodoro_skip(void)
{
  /* Skip current session: record focus time and start break */
  if (g_state == POMODORO_FOCUS)
    {
      int elapsed = g_focus_minutes * 60 - g_remaining_secs;
      if (elapsed > 60)
        study_record_add_minutes(elapsed / 60);

      g_today_count++;
      g_remaining_secs = g_break_minutes * 60;
      g_state = POMODORO_BREAK;
      g_last_tick = time(NULL);
      if (g_callback)
        g_callback(POMODORO_BREAK, g_subject);
      return 0;
    }

  if (g_state == POMODORO_BREAK)
    {
      /* Skip break: start next focus */
      g_remaining_secs = g_focus_minutes * 60;
      g_state = POMODORO_FOCUS;
      g_last_tick = time(NULL);
      if (g_callback)
        g_callback(POMODORO_FOCUS, g_subject);
      return 0;
    }

  return -1;
}

/* Called every second by the main loop to tick the timer */
int pomodoro_tick(void)
{
  if (g_state != POMODORO_FOCUS && g_state != POMODORO_BREAK)
    return 0;

  time_t now = time(NULL);
  int elapsed = (int)(now - g_last_tick);
  if (elapsed <= 0) return 0;

  g_last_tick = now;
  g_remaining_secs -= elapsed;
  if (g_remaining_secs < 0) g_remaining_secs = 0;

  /* Time's up */
  if (g_remaining_secs == 0)
    {
      if (g_state == POMODORO_FOCUS)
        {
          /* Focus session completed → record study time */
          study_record_add_minutes(g_focus_minutes);
          g_today_count++;
          g_remaining_secs = g_break_minutes * 60;
          g_state = POMODORO_BREAK;
          if (g_callback)
            g_callback(POMODORO_BREAK, g_subject);
        }
      else if (g_state == POMODORO_BREAK)
        {
          /* Break completed → back to idle */
          g_state = POMODORO_COMPLETED;
          if (g_callback)
            g_callback(POMODORO_COMPLETED, g_subject);
        }
    }

  return 0;
}

pomodoro_state_t pomodoro_get_state(void)
{
  return g_state;
}

subject_t pomodoro_get_subject(void)
{
  return g_subject;
}

const char *pomodoro_get_subject_name(subject_t subject)
{
  if (subject >= 0 && subject < sizeof(g_subject_names) / sizeof(g_subject_names[0]))
    return g_subject_names[subject];
  return "未知";
}

int pomodoro_get_remaining_seconds(void)
{
  return g_remaining_secs;
}

int pomodoro_get_today_count(void)
{
  return g_today_count;
}

int pomodoro_set_duration(int focus_minutes, int break_minutes)
{
  g_focus_minutes = focus_minutes;
  g_break_minutes = break_minutes;
  return 0;
}

void pomodoro_get_duration(int *focus_minutes, int *break_minutes)
{
  *focus_minutes = g_focus_minutes;
  *break_minutes = g_break_minutes;
}