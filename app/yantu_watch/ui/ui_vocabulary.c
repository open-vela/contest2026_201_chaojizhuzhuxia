/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_vocabulary.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include "ui_vocabulary.h"

lv_obj_t *ui_vocabulary_create(void)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "考研单词");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Word card */
  lv_obj_t *card = lv_obj_create(scr);
  lv_obj_set_size(card, 220, 160);
  lv_obj_align(card, LV_ALIGN_CENTER, 0, -10);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x16213e), 0);

  lv_obj_t *word_label = lv_label_create(card);
  lv_label_set_text(word_label, "abandon");
  lv_obj_align(word_label, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(word_label, lv_color_hex(0xe94560), 0);
  lv_obj_set_style_text_font(word_label, &lv_font_montserrat_14, 0);

  lv_obj_t *phonetic_label = lv_label_create(card);
  lv_label_set_text(phonetic_label, "/əˈbændən/");
  lv_obj_align(phonetic_label, LV_ALIGN_TOP_MID, 0, 40);
  lv_obj_set_style_text_color(phonetic_label, lv_color_hex(0x888888), 0);

  lv_obj_t *meaning_label = lv_label_create(card);
  lv_label_set_text(meaning_label, "v. 放弃，遗弃");
  lv_obj_align(meaning_label, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_set_style_text_color(meaning_label, lv_color_hex(0xaaaaaa), 0);

  /* Progress bar */
  lv_obj_t *progress = lv_label_create(scr);
  lv_label_set_text(progress, "0 / 0");
  lv_obj_align(progress, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_text_color(progress, lv_color_hex(0x888888), 0);

  return scr;
}

void ui_vocabulary_show_word(const char *word, const char *phonetic,
                             const char *meaning, const char *example)
{
  /* TODO: Update word display */
}

void ui_vocabulary_update_progress(int current, int total)
{
  /* TODO: Update progress display */
}