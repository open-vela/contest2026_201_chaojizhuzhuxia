/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_audio.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_audio.h"

lv_obj_t *ui_audio_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x0f3460), 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "音频播放");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  lv_obj_t *track_label = lv_label_create(scr);
  lv_label_set_text(track_label, "未选择音频");
  lv_obj_align(track_label, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_text_color(track_label, lv_color_hex(0xaaaaaa), 0);

  lv_obj_t *time_label = lv_label_create(scr);
  lv_label_set_text(time_label, "00:00 / 00:00");
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 10);
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x888888), 0);

  return scr;
}

void ui_audio_update_title(const char *title)
{
  /* TODO: Update track title */
}

void ui_audio_update_progress(int current_sec, int total_sec)
{
  /* TODO: Update progress display */
}

void ui_audio_set_playing(bool playing)
{
  /* TODO: Update play/pause state */
}