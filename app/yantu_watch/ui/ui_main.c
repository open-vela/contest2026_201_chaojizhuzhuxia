/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_main.c
 *
 * 主表盘界面 - 显示实时时钟、今日学习时长、快捷导航
 ****************************************************************************/

#include <nuttx/config.h>
#include <time.h>
#include <stdio.h>
#include <lvgl/lvgl.h>
#include "ui_main.h"
#include "ui_countdown.h"
#include "core/study_record.h"

/* Object references for live update */
static lv_obj_t *g_clock_label = NULL;
static lv_obj_t *g_study_label = NULL;
static lv_obj_t *g_countdown_btn = NULL;

lv_obj_t *ui_main_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);

  /* -- 实时时钟 (顶部) -- */
  g_clock_label = lv_label_create(scr);
  lv_label_set_text(g_clock_label, "00:00");
  lv_obj_align(g_clock_label, LV_ALIGN_TOP_MID, 0, 15);
  lv_obj_set_style_text_color(g_clock_label, lv_color_hex(0xffffff), 0);
  lv_obj_set_style_text_font(g_clock_label, &lv_font_montserrat_14, 0);

  /* -- 倒计时入口按钮 (中间) -- */
  g_countdown_btn = lv_btn_create(scr);
  lv_obj_set_size(g_countdown_btn, 200, 60);
  lv_obj_align(g_countdown_btn, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_bg_color(g_countdown_btn, lv_color_hex(0x533483), 0);

  lv_obj_t *btn_label = lv_label_create(g_countdown_btn);
  lv_label_set_text(btn_label, "距离考研\n还有 -- 天");
  lv_obj_center(btn_label);
  lv_obj_set_style_text_color(btn_label, lv_color_hex(0xffffff), 0);
  lv_obj_set_style_text_align(btn_label, LV_TEXT_ALIGN_CENTER, 0);

  /* -- 今日学习时长 (底部) -- */
  g_study_label = lv_label_create(scr);
  lv_label_set_text(g_study_label, "今日学习: 0 分钟");
  lv_obj_align(g_study_label, LV_ALIGN_BOTTOM_MID, 0, -15);
  lv_obj_set_style_text_color(g_study_label, lv_color_hex(0x00ff88), 0);

  return scr;
}

void ui_main_update_time(void)
{
  if (!g_clock_label) return;

  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d", tm->tm_hour, tm->tm_min);
  lv_label_set_text(g_clock_label, buf);
}

void ui_main_update_study_today(int minutes)
{
  if (!g_study_label) return;

  char buf[64];
  snprintf(buf, sizeof(buf), "今日学习: %d 分钟", minutes);
  lv_label_set_text(g_study_label, buf);
}

void ui_main_update_countdown_days(int days)
{
  if (!g_countdown_btn) return;

  lv_obj_t *btn_label = lv_obj_get_child(g_countdown_btn, 0);
  if (!btn_label) return;

  char buf[64];
  snprintf(buf, sizeof(buf), "距离考研\n还有 %d 天", days);
  lv_label_set_text(btn_label, buf);
}