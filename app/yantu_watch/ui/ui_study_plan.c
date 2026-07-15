/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_study_plan.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_study_plan.h"

lv_obj_t *ui_study_plan_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "复习计划");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  lv_obj_t *no_task = lv_label_create(scr);
  lv_label_set_text(no_task, "今日暂无任务");
  lv_obj_align(no_task, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(no_task, lv_color_hex(0x888888), 0);

  return scr;
}

void ui_study_plan_add_task(const char *task, bool done)
{
  /* TODO: Add task to plan list */
}

void ui_study_plan_clear(void)
{
  /* TODO: Clear plan list */
}