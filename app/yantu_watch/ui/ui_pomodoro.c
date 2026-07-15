/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_pomodoro.c
 *
 * 番茄自习计时器界面
 * - 圆形进度条显示剩余时间
 * - 科目切换按钮
 * - 开始/暂停/跳过控制
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <lvgl/lvgl.h>
#include "ui_pomodoro.h"
#include "core/pomodoro_timer.h"

/* Object references for live update */
static lv_obj_t *g_timer_label = NULL;
static lv_obj_t *g_subject_label = NULL;
static lv_obj_t *g_count_label = NULL;
static lv_obj_t *g_state_label = NULL;
static lv_obj_t *g_arc = NULL;
static lv_obj_t *g_btn_start = NULL;
static lv_obj_t *g_btn_stop = NULL;

/* Subject button group */
static lv_obj_t *g_subj_btns[5] = {NULL};

static const char *g_subject_labels[] = {
  "政治", "英语", "数学", "专业课", "自定义"
};

/* Subject button callback */
static void subject_btn_cb(lv_event_t *e)
{
  lv_obj_t *btn = lv_event_get_target(e);
  for (int i = 0; i < 5; i++)
    {
      if (g_subj_btns[i] == btn)
        {
          if (pomodoro_get_state() == POMODORO_IDLE)
            {
              pomodoro_start((subject_t)i);
              lv_label_set_text(g_state_label, "专注中");
              lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x00ff88), 0);
            }
          break;
        }
    }
}

/* Control button callbacks */
static void start_btn_cb(lv_event_t *e)
{
  pomodoro_state_t s = pomodoro_get_state();
  if (s == POMODORO_IDLE || s == POMODORO_COMPLETED)
    {
      pomodoro_start(SUBJECT_ENGLISH);
      lv_label_set_text(g_state_label, "专注中");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x00ff88), 0);
    }
  else if (s == POMODORO_BREAK)
    {
      pomodoro_resume();
      lv_label_set_text(g_state_label, "专注中");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x00ff88), 0);
    }
  else if (s == POMODORO_FOCUS)
    {
      pomodoro_pause();
      lv_label_set_text(g_state_label, "已暂停");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0xffaa00), 0);
    }
}

static void stop_btn_cb(lv_event_t *e)
{
  pomodoro_stop();
  lv_label_set_text(g_state_label, "空闲");
  lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x888888), 0);
  lv_label_set_text(g_timer_label, "25:00");
}

lv_obj_t *ui_pomodoro_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x0f3460), 0);

  /* Title */
  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "番茄自习");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* State label */
  g_state_label = lv_label_create(scr);
  lv_label_set_text(g_state_label, "空闲");
  lv_obj_align(g_state_label, LV_ALIGN_TOP_MID, 0, 25);
  lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x888888), 0);

  /* Circular progress arc */
  g_arc = lv_arc_create(scr);
  lv_obj_set_size(g_arc, 140, 140);
  lv_obj_align(g_arc, LV_ALIGN_CENTER, 0, -20);
  lv_arc_set_bg_angles(g_arc, 0, 360);
  lv_arc_set_angles(g_arc, 0, 0);
  lv_obj_set_style_arc_width(g_arc, 10, 0);
  lv_obj_set_style_arc_color(g_arc, lv_color_hex(0x333333), LV_PART_MAIN);
  lv_obj_set_style_arc_color(g_arc, lv_color_hex(0x00ff88), LV_PART_INDICATOR);
  lv_obj_remove_style(g_arc, NULL, LV_PART_KNOB);

  /* Timer display (center of arc) */
  g_timer_label = lv_label_create(scr);
  lv_label_set_text(g_timer_label, "25:00");
  lv_obj_align(g_timer_label, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_style_text_color(g_timer_label, lv_color_hex(0x00ff88), 0);

  /* Subject label */
  g_subject_label = lv_label_create(scr);
  lv_label_set_text(g_subject_label, "英语");
  lv_obj_align(g_subject_label, LV_ALIGN_CENTER, 0, 50);
  lv_obj_set_style_text_color(g_subject_label, lv_color_hex(0xaaaaaa), 0);

  /* Subject buttons (row) */
  for (int i = 0; i < 5; i++)
    {
      g_subj_btns[i] = lv_btn_create(scr);
      lv_obj_set_size(g_subj_btns[i], 40, 25);
      lv_obj_align(g_subj_btns[i], LV_ALIGN_BOTTOM_MID, (i - 2) * 45, -50);
      lv_obj_set_style_bg_color(g_subj_btns[i], lv_color_hex(0x533483), 0);
      lv_obj_add_event_cb(g_subj_btns[i], subject_btn_cb, LV_EVENT_CLICKED, NULL);

      lv_obj_t *lbl = lv_label_create(g_subj_btns[i]);
      lv_label_set_text(lbl, g_subject_labels[i]);
      lv_obj_center(lbl);
      lv_obj_set_style_text_color(lbl, lv_color_hex(0xffffff), 0);
    }

  /* Start / Pause button */
  g_btn_start = lv_btn_create(scr);
  lv_obj_set_size(g_btn_start, 70, 30);
  lv_obj_align(g_btn_start, LV_ALIGN_BOTTOM_MID, -40, -10);
  lv_obj_set_style_bg_color(g_btn_start, lv_color_hex(0x00ff88), 0);
  lv_obj_add_event_cb(g_btn_start, start_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *start_lbl = lv_label_create(g_btn_start);
  lv_label_set_text(start_lbl, "开始");
  lv_obj_center(start_lbl);

  /* Stop button */
  g_btn_stop = lv_btn_create(scr);
  lv_obj_set_size(g_btn_stop, 70, 30);
  lv_obj_align(g_btn_stop, LV_ALIGN_BOTTOM_MID, 40, -10);
  lv_obj_set_style_bg_color(g_btn_stop, lv_color_hex(0xe94560), 0);
  lv_obj_add_event_cb(g_btn_stop, stop_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *stop_lbl = lv_label_create(g_btn_stop);
  lv_label_set_text(stop_lbl, "停止");
  lv_obj_center(stop_lbl);

  /* Today's count */
  g_count_label = lv_label_create(scr);
  lv_label_set_text(g_count_label, "今日: 0 个番茄");
  lv_obj_align(g_count_label, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_obj_set_style_text_color(g_count_label, lv_color_hex(0x888888), 0);

  return scr;
}

void ui_pomodoro_update_time(int remaining_secs)
{
  if (!g_timer_label) return;
  int mins = remaining_secs / 60;
  int secs = remaining_secs % 60;
  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d", mins, secs);
  lv_label_set_text(g_timer_label, buf);

  /* Update arc progress (25 min = 1500 secs) */
  if (g_arc)
    {
      int total = 25 * 60;
      int angle = (total - remaining_secs) * 360 / total;
      if (angle > 360) angle = 360;
      lv_arc_set_angles(g_arc, 0, angle);
    }
}

void ui_pomodoro_update_state(int state)
{
  if (!g_state_label) return;
  switch ((pomodoro_state_t)state)
    {
    case POMODORO_IDLE:
      lv_label_set_text(g_state_label, "空闲");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x888888), 0);
      break;
    case POMODORO_FOCUS:
      lv_label_set_text(g_state_label, "专注中");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x00ff88), 0);
      break;
    case POMODORO_BREAK:
      lv_label_set_text(g_state_label, "休息中");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0xffaa00), 0);
      break;
    case POMODORO_COMPLETED:
      lv_label_set_text(g_state_label, "已完成 ✔");
      lv_obj_set_style_text_color(g_state_label, lv_color_hex(0x00ff88), 0);
      break;
    }
}

void ui_pomodoro_update_count(int count)
{
  if (!g_count_label) return;
  char buf[32];
  snprintf(buf, sizeof(buf), "今日: %d 个番茄", count);
  lv_label_set_text(g_count_label, buf);
}

void ui_pomodoro_update_subject(subject_t subject)
{
  if (!g_subject_label) return;
  lv_label_set_text(g_subject_label, pomodoro_get_subject_name(subject));
}