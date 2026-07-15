/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_countdown.c
 *
 * 考研倒计时页面 - 显示距离考研的精确倒计时
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <lvgl/lvgl.h>
#include "ui_countdown.h"

/* Object references for live update */
static lv_obj_t *g_days_label = NULL;
static lv_obj_t *g_detail_label = NULL;
static lv_obj_t *g_arc = NULL;

lv_obj_t *ui_countdown_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x16213e), 0);

  /* -- 标题 -- */
  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "考研倒计时");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* -- 进度环 (圆形) -- */
  g_arc = lv_arc_create(scr);
  lv_obj_set_size(g_arc, 180, 180);
  lv_obj_align(g_arc, LV_ALIGN_CENTER, 0, -10);
  lv_arc_set_bg_angles(g_arc, 0, 360);
  lv_arc_set_angles(g_arc, 0, 0);
  lv_obj_set_style_arc_width(g_arc, 12, 0);
  lv_obj_set_style_arc_color(g_arc, lv_color_hex(0x333333), LV_PART_MAIN);
  lv_obj_set_style_arc_color(g_arc, lv_color_hex(0xe94560), LV_PART_INDICATOR);
  lv_obj_remove_style(g_arc, NULL, LV_PART_KNOB); /* Hide knob */

  /* -- 天数显示 (环中间) -- */
  g_days_label = lv_label_create(scr);
  lv_label_set_text(g_days_label, "0");
  lv_obj_align(g_days_label, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_style_text_color(g_days_label, lv_color_hex(0xe94560), 0);
  lv_obj_set_style_text_font(g_days_label, &lv_font_montserrat_14, 0);

  lv_obj_t *days_unit = lv_label_create(scr);
  lv_label_set_text(days_unit, "天");
  lv_obj_align(days_unit, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(days_unit, lv_color_hex(0x888888), 0);

  /* -- 精确时间 (天/时/分/秒) -- */
  g_detail_label = lv_label_create(scr);
  lv_label_set_text(g_detail_label, "00天 00时 00分 00秒");
  lv_obj_align(g_detail_label, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_obj_set_style_text_color(g_detail_label, lv_color_hex(0xaaaaaa), 0);

  return scr;
}

void ui_countdown_update(int days, int hours, int mins, int secs)
{
  char buf[64];

  /* Update days number */
  snprintf(buf, sizeof(buf), "%d", days);
  if (g_days_label)
    lv_label_set_text(g_days_label, buf);

  /* Update detail string */
  snprintf(buf, sizeof(buf), "%d天 %02d时 %02d分 %02d秒",
           days, hours, mins, secs);
  if (g_detail_label)
    lv_label_set_text(g_detail_label, buf);

  /* Update arc progress (assume 365 days max) */
  if (g_arc)
    {
      int max_days = 365;
      int angle = (max_days - days > 0)
                  ? (360 * (max_days - days) / max_days)
                  : 360;
      if (angle > 360) angle = 360;
      lv_arc_set_angles(g_arc, 0, angle);
    }
}