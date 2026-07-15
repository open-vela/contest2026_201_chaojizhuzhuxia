/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_settings.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_settings.h"

lv_obj_t *ui_settings_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x0f3460), 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "设置");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  lv_obj_t *version = lv_label_create(scr);
  lv_label_set_text(version, "研途计时 v1.0.0");
  lv_obj_align(version, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_text_color(version, lv_color_hex(0x888888), 0);

  return scr;
}