/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_vocabulary.c
 *
 * 考研单词背诵界面
 * - 单词卡片：点击翻转（英文→中文释义）
 * - 上/下一个单词导航
 * - 标记按钮：已掌握/需复习/收藏
 * - 词库选择：四级/六级/考研
 * - 进度统计
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <lvgl/lvgl.h>
#include "ui_vocabulary.h"
#include "core/vocabulary_engine.h"

/* 词库列表 */
static const char *g_vocab_lists[] = {
  "kaoyan.json",
};
static const char *g_vocab_labels[] = {
  "考研词汇",
};
#define VOCAB_LIST_COUNT 1

/* Object references */
static lv_obj_t *g_scr = NULL;

/* Card elements */
static lv_obj_t *g_card = NULL;
static lv_obj_t *g_word_label = NULL;
static lv_obj_t *g_phonetic_label = NULL;
static lv_obj_t *g_meaning_label = NULL;
static lv_obj_t *g_example_label = NULL;

/* State */
static lv_obj_t *g_state_label = NULL;

/* Navigation */
static lv_obj_t *g_prev_btn = NULL;
static lv_obj_t *g_next_btn = NULL;

/* Mark buttons */
static lv_obj_t *g_btn_mastered = NULL;
static lv_obj_t *g_btn_review = NULL;
static lv_obj_t *g_btn_fav = NULL;

/* Progress */
static lv_obj_t *g_progress_label = NULL;
static lv_obj_t *g_count_label = NULL;

/* List selection */
static lv_obj_t *g_list_btn = NULL;

/* Internal state */
static bool g_show_meaning = false;

/****************************************************************************
 * Refresh the display from vocabulary engine
 ****************************************************************************/

static void refresh_display(void)
{
  int idx = vocab_get_current_index();
  int total = vocab_get_count();
  word_t word;

  if (total <= 0)
    {
      lv_label_set_text(g_word_label, "未加载词库");
      lv_label_set_text(g_phonetic_label, "");
      lv_label_set_text(g_meaning_label, "请点击上方选择词库");
      lv_label_set_text(g_example_label, "");
      lv_label_set_text(g_progress_label, "0 / 0");
      lv_label_set_text(g_count_label, "已掌握: 0 | 待复习: 0");
      return;
    }

  if (vocab_get_word(idx, &word) != 0)
    return;

  /* Update word display based on flip state */
  if (g_show_meaning)
    {
      /* Show Chinese side */
      lv_label_set_text(g_word_label, word.meaning);
      lv_obj_set_style_text_color(g_word_label, lv_color_hex(0x00ff88), 0);
      lv_label_set_text(g_phonetic_label, "");
      lv_label_set_text(g_meaning_label, word.example);
      lv_label_set_text(g_example_label, "");
    }
  else
    {
      /* Show English side */
      lv_label_set_text(g_word_label, word.word);
      lv_obj_set_style_text_color(g_word_label, lv_color_hex(0xe94560), 0);
      lv_label_set_text(g_phonetic_label, word.phonetic);
      lv_label_set_text(g_meaning_label, word.meaning);
      lv_label_set_text(g_example_label, word.example);
    }

  /* Update progress */
  char buf[32];
  snprintf(buf, sizeof(buf), "%d / %d", idx + 1, total);
  lv_label_set_text(g_progress_label, buf);

  /* Update statistics */
  int mastered = vocab_get_mastered_count();
  int review = vocab_get_review_count();
  snprintf(buf, sizeof(buf), "已掌握: %d | 待复习: %d", mastered, review);
  lv_label_set_text(g_count_label, buf);

  /* Update mark button states */
  int mark = vocab_get_mark(idx);
  lv_obj_set_style_bg_color(g_btn_mastered,
    lv_color_hex(mark == 1 ? 0x00aa44 : 0x333333), 0);
  lv_obj_set_style_bg_color(g_btn_review,
    lv_color_hex(mark == 2 ? 0xaa6600 : 0x333333), 0);
  lv_obj_set_style_bg_color(g_btn_fav,
    lv_color_hex(mark == 3 ? 0xaa0044 : 0x333333), 0);
}

/****************************************************************************
 * Card tap callback — flip between English and Chinese
 ****************************************************************************/

static void card_click_cb(lv_event_t *e)
{
  g_show_meaning = !g_show_meaning;
  refresh_display();
}

/****************************************************************************
 * Navigation callbacks
 ****************************************************************************/

static void prev_btn_cb(lv_event_t *e)
{
  g_show_meaning = false;
  if (vocab_prev() == 0)
    refresh_display();
}

static void next_btn_cb(lv_event_t *e)
{
  g_show_meaning = false;
  if (vocab_next() == 0)
    refresh_display();
}

/****************************************************************************
 * Mark button callbacks
 ****************************************************************************/

static void mastered_btn_cb(lv_event_t *e)
{
  int idx = vocab_get_current_index();
  int cur = vocab_get_mark(idx);
  int new_mark = (cur == 1) ? 0 : 1;
  vocab_set_mark(idx, new_mark);
  refresh_display();
}

static void review_btn_cb(lv_event_t *e)
{
  int idx = vocab_get_current_index();
  int cur = vocab_get_mark(idx);
  int new_mark = (cur == 2) ? 0 : 2;
  vocab_set_mark(idx, new_mark);
  refresh_display();
}

static void fav_btn_cb(lv_event_t *e)
{
  int idx = vocab_get_current_index();
  int cur = vocab_get_mark(idx);
  int new_mark = (cur == 3) ? 0 : 3;
  vocab_set_mark(idx, new_mark);
  refresh_display();
}

/****************************************************************************
 * List selection callback
 ****************************************************************************/

static void list_btn_cb(lv_event_t *e)
{
  /* Cycle through available word lists */
  static int list_idx = 0;
  list_idx = (list_idx + 1) % VOCAB_LIST_COUNT;

  g_show_meaning = false;
  vocab_init();
  vocab_load_list(g_vocab_lists[list_idx]);
  lv_label_set_text(g_list_btn, g_vocab_labels[list_idx]);
  refresh_display();
}

/****************************************************************************
 * Create the vocabulary screen
 ****************************************************************************/

lv_obj_t *ui_vocabulary_create(void)
{
  g_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(g_scr, lv_color_hex(0x1a1a2e), 0);

  /* ===== 词库选择按钮（顶部） ===== */
  g_list_btn = lv_btn_create(g_scr);
  lv_obj_set_size(g_list_btn, 100, 24);
  lv_obj_align(g_list_btn, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_bg_color(g_list_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(g_list_btn, list_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *list_lbl = lv_label_create(g_list_btn);
  lv_label_set_text(list_lbl, "考研词汇");
  lv_obj_center(list_lbl);
  lv_obj_set_style_text_color(list_lbl, lv_color_hex(0xffffff), 0);

  /* ===== 单词卡片（可点击翻转） ===== */
  g_card = lv_obj_create(g_scr);
  lv_obj_set_size(g_card, 220, 150);
  lv_obj_align(g_card, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_style_bg_color(g_card, lv_color_hex(0x16213e), 0);
  lv_obj_set_style_border_width(g_card, 1, 0);
  lv_obj_set_style_border_color(g_card, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(g_card, card_click_cb, LV_EVENT_CLICKED, NULL);

  /* Word (English or Chinese) */
  g_word_label = lv_label_create(g_card);
  lv_label_set_text(g_word_label, "abandon");
  lv_obj_align(g_word_label, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_color(g_word_label, lv_color_hex(0xe94560), 0);
  lv_obj_set_style_text_font(g_word_label, &lv_font_montserrat_14, 0);

  /* Phonetic */
  g_phonetic_label = lv_label_create(g_card);
  lv_label_set_text(g_phonetic_label, "/əˈbændən/");
  lv_obj_align(g_phonetic_label, LV_ALIGN_TOP_MID, 0, 35);
  lv_obj_set_style_text_color(g_phonetic_label, lv_color_hex(0x888888), 0);

  /* Meaning */
  g_meaning_label = lv_label_create(g_card);
  lv_label_set_text(g_meaning_label, "v. 放弃，遗弃");
  lv_obj_align(g_meaning_label, LV_ALIGN_TOP_MID, 0, 60);
  lv_obj_set_style_text_color(g_meaning_label, lv_color_hex(0xaaaaaa), 0);
  lv_label_set_long_mode(g_meaning_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(g_meaning_label, 200);

  /* Example sentence */
  g_example_label = lv_label_create(g_card);
  lv_label_set_text(g_example_label, "He abandoned his plan.");
  lv_obj_align(g_example_label, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_text_color(g_example_label, lv_color_hex(0x666666), 0);
  lv_label_set_long_mode(g_example_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(g_example_label, 200);

  /* Hint text */
  lv_obj_t *hint = lv_label_create(g_scr);
  lv_label_set_text(hint, "点击卡片翻转");
  lv_obj_align(hint, LV_ALIGN_CENTER, 0, -80);
  lv_obj_set_style_text_color(hint, lv_color_hex(0x555555), 0);

  /* ===== Navigation buttons ===== */
  /* Previous button */
  g_prev_btn = lv_btn_create(g_scr);
  lv_obj_set_size(g_prev_btn, 50, 30);
  lv_obj_align(g_prev_btn, LV_ALIGN_CENTER, -70, 70);
  lv_obj_set_style_bg_color(g_prev_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(g_prev_btn, prev_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *prev_lbl = lv_label_create(g_prev_btn);
  lv_label_set_text(prev_lbl, "◀");
  lv_obj_center(prev_lbl);
  lv_obj_set_style_text_color(prev_lbl, lv_color_hex(0xffffff), 0);

  /* Next button */
  g_next_btn = lv_btn_create(g_scr);
  lv_obj_set_size(g_next_btn, 50, 30);
  lv_obj_align(g_next_btn, LV_ALIGN_CENTER, 70, 70);
  lv_obj_set_style_bg_color(g_next_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(g_next_btn, next_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *next_lbl = lv_label_create(g_next_btn);
  lv_label_set_text(next_lbl, "▶");
  lv_obj_center(next_lbl);
  lv_obj_set_style_text_color(next_lbl, lv_color_hex(0xffffff), 0);

  /* ===== Progress label (between nav buttons) ===== */
  g_progress_label = lv_label_create(g_scr);
  lv_label_set_text(g_progress_label, "0 / 0");
  lv_obj_align(g_progress_label, LV_ALIGN_CENTER, 0, 70);
  lv_obj_set_style_text_color(g_progress_label, lv_color_hex(0x888888), 0);

  /* ===== Mark buttons (bottom) ===== */
  /* 已掌握 */
  g_btn_mastered = lv_btn_create(g_scr);
  lv_obj_set_size(g_btn_mastered, 60, 26);
  lv_obj_align(g_btn_mastered, LV_ALIGN_BOTTOM_MID, -65, -35);
  lv_obj_set_style_bg_color(g_btn_mastered, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(g_btn_mastered, mastered_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *m_lbl = lv_label_create(g_btn_mastered);
  lv_label_set_text(m_lbl, "已掌握");
  lv_obj_center(m_lbl);
  lv_obj_set_style_text_color(m_lbl, lv_color_hex(0xffffff), 0);

  /* 需复习 */
  g_btn_review = lv_btn_create(g_scr);
  lv_obj_set_size(g_btn_review, 60, 26);
  lv_obj_align(g_btn_review, LV_ALIGN_BOTTOM_MID, 0, -35);
  lv_obj_set_style_bg_color(g_btn_review, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(g_btn_review, review_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *r_lbl = lv_label_create(g_btn_review);
  lv_label_set_text(r_lbl, "需复习");
  lv_obj_center(r_lbl);
  lv_obj_set_style_text_color(r_lbl, lv_color_hex(0xffffff), 0);

  /* 收藏 */
  g_btn_fav = lv_btn_create(g_scr);
  lv_obj_set_size(g_btn_fav, 60, 26);
  lv_obj_align(g_btn_fav, LV_ALIGN_BOTTOM_MID, 65, -35);
  lv_obj_set_style_bg_color(g_btn_fav, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(g_btn_fav, fav_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *f_lbl = lv_label_create(g_btn_fav);
  lv_label_set_text(f_lbl, "收藏");
  lv_obj_center(f_lbl);
  lv_obj_set_style_text_color(f_lbl, lv_color_hex(0xffffff), 0);

  /* ===== Statistics (very bottom) ===== */
  g_count_label = lv_label_create(g_scr);
  lv_label_set_text(g_count_label, "已掌握: 0 | 待复习: 0");
  lv_obj_align(g_count_label, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_text_color(g_count_label, lv_color_hex(0x666666), 0);

  return g_scr;
}

/****************************************************************************
 * Public: refresh from external (e.g. main loop)
 ****************************************************************************/

void ui_vocabulary_refresh(void)
{
  refresh_display();
}

void ui_vocabulary_update_progress(int current, int total)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%d / %d", current, total);
  if (g_progress_label)
    lv_label_set_text(g_progress_label, buf);
}