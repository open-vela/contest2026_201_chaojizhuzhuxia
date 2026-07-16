/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_reminder.c
 *
 * 定时提醒界面
 * - 提醒列表显示（时间、标签、重复、开关）
 * - 添加新提醒（时间选择器）
 * - 删除提醒
 * - 开关控制
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <lvgl/lvgl.h>
#include "ui_reminder.h"
#include "core/reminder_manager.h"

/* Object references */
static lv_obj_t *g_scr = NULL;
static lv_obj_t *g_container = NULL;
static lv_obj_t *g_empty_label = NULL;
static lv_obj_t *g_add_btn = NULL;

/* Add dialog state */
static lv_obj_t *g_dialog = NULL;
static lv_obj_t *g_hour_roller = NULL;
static lv_obj_t *g_min_roller = NULL;
static lv_obj_t *g_label_ta = NULL;

/* Forward declarations */
static void toggle_btn_cb(lv_event_t *e);
static void delete_btn_cb(lv_event_t *e);

/****************************************************************************
 * Rebuild the reminder list UI from engine data
 ****************************************************************************/

static void rebuild_list(void)
{
  /* Clear existing items */
  if (g_container)
    lv_obj_clean(g_container);

  int count = reminder_get_count();

  if (count == 0)
    {
      lv_obj_clear_flag(g_empty_label, LV_OBJ_FLAG_HIDDEN);
      return;
    }

  lv_obj_add_flag(g_empty_label, LV_OBJ_FLAG_HIDDEN);

  for (int i = 0; i < count; i++)
    {
      reminder_item_t item;
      if (reminder_get_item(i, &item) != 0) continue;

      /* Create a row for each reminder */
      lv_obj_t *row = lv_obj_create(g_container);
      lv_obj_set_size(row, 220, 36);
      lv_obj_set_style_bg_color(row, lv_color_hex(0x16213e), 0);
      lv_obj_set_style_border_width(row, 0, 0);
      lv_obj_set_style_pad_all(row, 2, 0);
      lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
      lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

      /* Time label */
      char time_buf[16];
      snprintf(time_buf, sizeof(time_buf), "%02d:%02d", item.hour, item.minute);
      lv_obj_t *time_lbl = lv_label_create(row);
      lv_label_set_text(time_lbl, time_buf);
      lv_obj_set_style_text_color(time_lbl,
        item.enabled ? lv_color_hex(0x00ff88) : lv_color_hex(0x666666), 0);

      /* Label text */
      lv_obj_t *label_lbl = lv_label_create(row);
      lv_label_set_text(label_lbl, item.label);
      lv_obj_set_style_text_color(label_lbl,
        item.enabled ? lv_color_hex(0xffffff) : lv_color_hex(0x666666), 0);

      /* Repeat indicator */
      lv_obj_t *rep_lbl = lv_label_create(row);
      lv_label_set_text(rep_lbl, item.repeat ? "每天" : "一次");
      lv_obj_set_style_text_color(rep_lbl, lv_color_hex(0x888888), 0);

      /* Enable/Disable toggle button */
      lv_obj_t *toggle_btn = lv_btn_create(row);
      lv_obj_set_size(toggle_btn, 40, 26);
      lv_obj_set_style_bg_color(toggle_btn,
        item.enabled ? lv_color_hex(0x00aa44) : lv_color_hex(0x555555), 0);

      lv_obj_t *t_lbl = lv_label_create(toggle_btn);
      lv_label_set_text(t_lbl, item.enabled ? "开" : "关");
      lv_obj_center(t_lbl);
      lv_obj_set_style_text_color(t_lbl, lv_color_hex(0xffffff), 0);

      /* Store reminder ID in user_data */
      int *id_ptr = (int *)malloc(sizeof(int));
      *id_ptr = item.id;
      lv_obj_set_user_data(toggle_btn, id_ptr);
      lv_obj_add_event_cb(toggle_btn, toggle_btn_cb, LV_EVENT_CLICKED, NULL);

      /* Delete button */
      lv_obj_t *del_btn = lv_btn_create(row);
      lv_obj_set_size(del_btn, 30, 26);
      lv_obj_set_style_bg_color(del_btn, lv_color_hex(0xaa3333), 0);

      lv_obj_t *d_lbl = lv_label_create(del_btn);
      lv_label_set_text(d_lbl, "✕");
      lv_obj_center(d_lbl);
      lv_obj_set_style_text_color(d_lbl, lv_color_hex(0xffffff), 0);

      int *del_id = (int *)malloc(sizeof(int));
      *del_id = item.id;
      lv_obj_set_user_data(del_btn, del_id);
      lv_obj_add_event_cb(del_btn, delete_btn_cb, LV_EVENT_CLICKED, NULL);
    }
}

/* Toggle button callback */
static void toggle_btn_cb(lv_event_t *e)
{
  lv_obj_t *btn = lv_event_get_target(e);
  int *id = (int *)lv_obj_get_user_data(btn);
  if (id)
    {
      reminder_item_t item;
      if (reminder_get_item_by_id(*id, &item) == 0)
        reminder_set_enabled(*id, !item.enabled);
      rebuild_list();
    }
}

/* Delete button callback */
static void delete_btn_cb(lv_event_t *e)
{
  lv_obj_t *btn = lv_event_get_target(e);
  int *id = (int *)lv_obj_get_user_data(btn);
  if (id)
    {
      reminder_remove(*id);
      free(id);
      rebuild_list();
    }
}

/****************************************************************************
 * Add reminder dialog
 ****************************************************************************/

static void dialog_confirm_cb(lv_event_t *e)
{
  if (g_dialog)
    {
      lv_obj_del(g_dialog);
      g_dialog = NULL;
    }

  /* Read roller values */
  char buf[8];
  lv_roller_get_selected_str(g_hour_roller, buf, sizeof(buf));
  int hour = atoi(buf);
  lv_roller_get_selected_str(g_min_roller, buf, sizeof(buf));
  int min = atoi(buf);

  /* Read label text (LVGL textarea) */
  const char *label = lv_textarea_get_text(g_label_ta);
  if (strlen(label) == 0) label = "自定义提醒";

  reminder_add(label, hour, min, true);
  rebuild_list();
}

static void dialog_cancel_cb(lv_event_t *e)
{
  if (g_dialog)
    {
      lv_obj_del(g_dialog);
      g_dialog = NULL;
    }
}

static void add_btn_cb(lv_event_t *e)
{
  if (g_dialog) return; /* Already open */

  g_dialog = lv_obj_create(lv_scr_act());
  lv_obj_set_size(g_dialog, 220, 200);
  lv_obj_center(g_dialog);
  lv_obj_set_style_bg_color(g_dialog, lv_color_hex(0x0f3460), 0);
  lv_obj_set_style_border_width(g_dialog, 2, 0);
  lv_obj_set_style_border_color(g_dialog, lv_color_hex(0x533483), 0);

  lv_obj_t *title = lv_label_create(g_dialog);
  lv_label_set_text(title, "添加新提醒");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Hour roller */
  g_hour_roller = lv_roller_create(g_dialog);
  lv_roller_set_options(g_hour_roller, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
                        LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(g_hour_roller, 8, LV_ANIM_OFF); /* Default 08 */
  lv_obj_set_size(g_hour_roller, 50, 80);
  lv_obj_align(g_hour_roller, LV_ALIGN_CENTER, -40, 10);

  /* Minute roller */
  g_min_roller = lv_roller_create(g_dialog);
  lv_roller_set_options(g_min_roller, "00\n05\n10\n15\n20\n25\n30\n35\n40\n45\n50\n55",
                        LV_ROLLER_MODE_NORMAL);
  lv_obj_set_size(g_min_roller, 50, 80);
  lv_obj_align(g_min_roller, LV_ALIGN_CENTER, 15, 10);

  /* Label textarea */
  g_label_ta = lv_textarea_create(g_dialog);
  lv_textarea_set_placeholder_text(g_label_ta, "提醒名称");
  lv_obj_set_size(g_label_ta, 120, 30);
  lv_obj_align(g_label_ta, LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_text_color(g_label_ta, lv_color_hex(0xffffff), 0);

  /* Confirm button */
  lv_obj_t *confirm = lv_btn_create(g_dialog);
  lv_obj_set_size(confirm, 70, 28);
  lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, -40, -5);
  lv_obj_set_style_bg_color(confirm, lv_color_hex(0x00aa44), 0);
  lv_obj_add_event_cb(confirm, dialog_confirm_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *c_lbl = lv_label_create(confirm);
  lv_label_set_text(c_lbl, "确认");
  lv_obj_center(c_lbl);

  /* Cancel button */
  lv_obj_t *cancel = lv_btn_create(g_dialog);
  lv_obj_set_size(cancel, 70, 28);
  lv_obj_align(cancel, LV_ALIGN_BOTTOM_MID, 40, -5);
  lv_obj_set_style_bg_color(cancel, lv_color_hex(0x666666), 0);
  lv_obj_add_event_cb(cancel, dialog_cancel_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *x_lbl = lv_label_create(cancel);
  lv_label_set_text(x_lbl, "取消");
  lv_obj_center(x_lbl);
}

/****************************************************************************
 * Create the reminder screen
 ****************************************************************************/

lv_obj_t *ui_reminder_create(void)
{
  g_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(g_scr, lv_color_hex(0x16213e), 0);

  /* Title */
  lv_obj_t *title = lv_label_create(g_scr);
  lv_label_set_text(title, "定时提醒");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Empty state label */
  g_empty_label = lv_label_create(g_scr);
  lv_label_set_text(g_empty_label, "暂无提醒\n点击下方 + 添加");
  lv_obj_align(g_empty_label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(g_empty_label, lv_color_hex(0x888888), 0);
  lv_obj_set_style_text_align(g_empty_label, LV_TEXT_ALIGN_CENTER, 0);

  /* Scrollable container for reminder list */
  g_container = lv_obj_create(g_scr);
  lv_obj_set_size(g_container, 230, 180);
  lv_obj_align(g_container, LV_ALIGN_CENTER, 0, -5);
  lv_obj_set_style_bg_color(g_container, lv_color_hex(0x16213e), 0);
  lv_obj_set_style_border_width(g_container, 0, 0);
  lv_obj_set_flex_flow(g_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(g_container, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_scrollbar_mode(g_container, LV_SCROLLBAR_MODE_AUTO);

  /* Add button */
  g_add_btn = lv_btn_create(g_scr);
  lv_obj_set_size(g_add_btn, 50, 30);
  lv_obj_align(g_add_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_bg_color(g_add_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(g_add_btn, add_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *add_lbl = lv_label_create(g_add_btn);
  lv_label_set_text(add_lbl, "＋ 添加");
  lv_obj_center(add_lbl);
  lv_obj_set_style_text_color(add_lbl, lv_color_hex(0xffffff), 0);

  /* Build the initial list */
  rebuild_list();

  return g_scr;
}

/****************************************************************************
 * Public API
 ****************************************************************************/

void ui_reminder_add_item(const char *label, int hour, int minute, bool repeat)
{
  reminder_add(label, hour, minute, repeat);
  rebuild_list();
}

void ui_reminder_clear(void)
{
  /* Clear all reminders */
  while (reminder_get_count() > 0)
    {
      reminder_item_t item;
      if (reminder_get_item(0, &item) == 0)
        reminder_remove(item.id);
    }
  rebuild_list();
}