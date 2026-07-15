/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_components.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_components.h"

lv_obj_t *ui_comp_circular_progress(lv_obj_t *parent, int size)
{
  lv_obj_t *arc = lv_arc_create(parent);
  lv_obj_set_size(arc, size, size);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_arc_set_angles(arc, 0, 0);
  lv_obj_set_style_arc_width(arc, 8, 0);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x333333), LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x00ff88), LV_PART_INDICATOR);
  return arc;
}

lv_obj_t *ui_comp_nav_button(lv_obj_t *parent, const char *label,
                             lv_event_cb_t cb)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 80, 40);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, label);
  lv_obj_center(lbl);
  lv_obj_set_style_text_color(lbl, lv_color_hex(0xffffff), 0);

  return btn;
}

lv_obj_t *ui_comp_page_title(lv_obj_t *parent, const char *title)
{
  lv_obj_t *lbl = lv_label_create(parent);
  lv_label_set_text(lbl, title);
  lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(lbl, lv_color_hex(0xffffff), 0);
  return lbl;
}

lv_obj_t *ui_comp_card(lv_obj_t *parent, int w, int h)
{
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_size(card, w, h);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x16213e), 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, lv_color_hex(0x333333), 0);
  return card;
}