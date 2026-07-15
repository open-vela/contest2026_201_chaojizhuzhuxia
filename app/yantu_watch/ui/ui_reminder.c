/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_reminder.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_reminder.h"

lv_obj_t *ui_reminder_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x16213e), 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "定时提醒");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  lv_obj_t *no_reminder = lv_label_create(scr);
  lv_label_set_text(no_reminder, "暂无提醒");
  lv_obj_align(no_reminder, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(no_reminder, lv_color_hex(0x888888), 0);

  return scr;
}

void ui_reminder_add_item(const char *label, int hour, int minute, bool repeat)
{
  /* TODO: Add reminder item to list */
}

void ui_reminder_clear(void)
{
  /* TODO: Clear reminder list */
}