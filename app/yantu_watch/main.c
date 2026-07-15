/****************************************************************************
 * apps/examples/yantu_watch/main.c
 *
 * 研途计时・离线考研智能穿戴手表 - 主入口
 * 负责 LVGL 初始化、页面管理、定时刷新
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/boardctl.h>

#include <lvgl/lvgl.h>

#include "ui/ui_main.h"
#include "ui/ui_countdown.h"
#include "ui/ui_pomodoro.h"
#include "ui/ui_vocabulary.h"
#include "ui/ui_audio.h"
#include "ui/ui_reminder.h"
#include "ui/ui_study_plan.h"
#include "ui/ui_settings.h"
#include "core/countdown_manager.h"
#include "core/pomodoro_timer.h"
#include "core/study_record.h"
#include "core/vibrator.h"
#include "data/storage.h"

#define YANTU_WATCH_VERSION "1.0.0"
#define LCD_H_RES          240
#define LCD_V_RES          240
#define REFRESH_PERIOD_MS  1000  /* Refresh every second */

/* Page IDs */
typedef enum
{
  PAGE_MAIN,
  PAGE_COUNTDOWN,
  PAGE_POMODORO,
  PAGE_VOCABULARY,
  PAGE_AUDIO,
  PAGE_REMINDER,
  PAGE_STUDY_PLAN,
  PAGE_SETTINGS,
  PAGE_MAX
} page_id_t;

/* Application state */
static struct
{
  lv_obj_t *screens[PAGE_MAX];
  page_id_t current_page;
  lv_timer_t *refresh_timer;
  bool initialized;
} g_app;

/****************************************************************************
 * LVGL Tick Timer
 ****************************************************************************/

static void lvgl_tick_cb(lv_timer_t *timer)
{
  lv_tick_inc(1);
}

/****************************************************************************
 * Periodic Refresh (1 second interval)
 ****************************************************************************/

static void refresh_cb(lv_timer_t *timer)
{
  /* Update clock on main screen */
  ui_main_update_time();

  /* Update countdown */
  int days, hours, mins, secs;
  countdown_get_time_remaining(&days, &hours, &mins, &secs);
  ui_countdown_update(days, hours, mins, secs);
  ui_main_update_countdown_days(days);

  /* Update study time on main screen */
  ui_main_update_study_today(study_record_get_today_minutes());

  /* Tick pomodoro timer (handles auto-transitions) */
  pomodoro_tick();

  /* Update pomodoro UI if visible */
  pomodoro_state_t ps = pomodoro_get_state();
  ui_pomodoro_update_time(pomodoro_get_remaining_seconds());
  ui_pomodoro_update_state(ps);
  ui_pomodoro_update_count(pomodoro_get_today_count());
  ui_pomodoro_update_subject(pomodoro_get_subject());
}

/****************************************************************************
 * Pomodoro State Change Callback (for vibration)
 ****************************************************************************/

static void pomodoro_callback(pomodoro_state_t state, subject_t subject)
{
  switch (state)
    {
    case POMODORO_BREAK:
      /* Focus finished → triple buzz */
      vibrator_set(VIBRATE_TRIPLE);
      break;
    case POMODORO_COMPLETED:
      /* Break finished → long buzz */
      vibrator_set(VIBRATE_LONG);
      break;
    default:
      break;
    }
}

/****************************************************************************
 * Page Switching
 ****************************************************************************/

static void page_switch(page_id_t target)
{
  if (target >= PAGE_MAX) return;
  if (g_app.screens[target] == NULL) return;

  g_app.current_page = target;
  lv_scr_load(g_app.screens[target]);
}

/* Navigation button callback: switch to countdown page */
static void nav_to_countdown_cb(lv_event_t *e)
{
  page_switch(PAGE_COUNTDOWN);
}

/* Back button callback: return to main page */
static void nav_back_cb(lv_event_t *e)
{
  page_switch(PAGE_MAIN);
}

/****************************************************************************
 * LVGL Initialization
 ****************************************************************************/

static void lvgl_init(void)
{
  lv_init();

  /* Create display for SF32LB52 LCD (240x240) */
  lv_display_t *disp = lv_display_create(LCD_H_RES, LCD_V_RES);
  if (disp)
    lv_display_set_default(disp);

  /* Create touchscreen input device */
  lv_indev_t *indev = lv_indev_create();
  if (indev)
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  /* Create a periodic tick timer for LVGL */
  lv_timer_create(lvgl_tick_cb, 1, NULL);
}

/****************************************************************************
 * Application Module Initialization
 ****************************************************************************/

static void app_init_modules(void)
{
  storage_init();
  countdown_init();
  pomodoro_init();
  study_record_init();
  vibrator_init();

  /* Register pomodoro callback for vibration alerts */
  pomodoro_register_callback(pomodoro_callback);
}

/****************************************************************************
 * Create All Screens
 ****************************************************************************/

static void create_all_screens(void)
{
  /* Main screen */
  g_app.screens[PAGE_MAIN] = ui_main_create();

  /* Countdown screen */
  g_app.screens[PAGE_COUNTDOWN] = ui_countdown_create();

  /* Other screens (placeholders for now) */
  g_app.screens[PAGE_POMODORO]   = ui_pomodoro_create();
  g_app.screens[PAGE_VOCABULARY] = ui_vocabulary_create();
  g_app.screens[PAGE_AUDIO]      = ui_audio_create();
  g_app.screens[PAGE_REMINDER]   = ui_reminder_create();
  g_app.screens[PAGE_STUDY_PLAN] = ui_study_plan_create();
  g_app.screens[PAGE_SETTINGS]   = ui_settings_create();
}

/****************************************************************************
 * Main Entry
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  printf("YanTu Watch v%s starting...\n", YANTU_WATCH_VERSION);

  /* Initialize subsystems */
  lvgl_init();
  app_init_modules();

  /* Create all UI screens */
  create_all_screens();

  /* Switch to main screen */
  g_app.current_page = PAGE_MAIN;
  lv_scr_load(g_app.screens[PAGE_MAIN]);

  /* Create periodic refresh timer */
  g_app.refresh_timer = lv_timer_create(refresh_cb, REFRESH_PERIOD_MS, NULL);

  printf("YanTu Watch initialized successfully!\n");

  /* Main loop */
  while (1)
  {
    lv_timer_handler();
    usleep(5000);
  }

  return 0;
}