/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_study_plan.c
 *
 * 复习计划界面
 * - 今日任务列表（checkbox 打卡）
 * - 添加新任务（科目选择 + 内容输入）
 * - 删除任务
 * - 完成进度统计
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <lvgl/lvgl.h>
#include "ui_study_plan.h"
#include "core/study_plan_manager.h"

/* Object references */
static lv_obj_t *g_scr = NULL;
static lv_obj_t *g_container = NULL;
static lv_obj_t *g_empty_label = NULL;
static lv_obj_t *g_progress_label = NULL;

/* Dialog state */
static lv_obj_t *g_dialog = NULL;
static lv_obj_t *g_subj_dd = NULL;
static lv_obj_t *g_content_ta = NULL;

static const char *g_subjects[] = {
  "政治", "英语", "数学", "专业课", "自定义"
};
#define SUBJECT_COUNT 5

/* Forward declarations */
static void checkbox_cb(lv_event_t *e);
static void delete_cb(lv_event_t *e);

/****************************************************************************
 * Rebuild the task list UI
 ****************************************************************************/

static void rebuild_list(void)
{
  if (g_container)
    lv_obj_clean(g_container);

  plan_task_t today_tasks[PLAN_MAX_TASKS];
  int count = PLAN_MAX_TASKS;
  plan_get_today_tasks(today_tasks, &count);

  int done_count = 0;

  if (count == 0)
    {
      lv_obj_clear_flag(g_empty_label, LV_OBJ_FLAG_HIDDEN);
      if (g_progress_label)
        lv_label_set_text(g_progress_label, "今日暂无任务");
      return;
    }

  lv_obj_add_flag(g_empty_label, LV_OBJ_FLAG_HIDDEN);

  for (int i = 0; i < count; i++)
    {
      if (today_tasks[i].done) done_count++;

      /* Row container */
      lv_obj_t *row = lv_obj_create(g_container);
      lv_obj_set_size(row, 220, 34);
      lv_obj_set_style_bg_color(row, lv_color_hex(0x1a1a2e), 0);
      lv_obj_set_style_border_width(row, 0, 0);
      lv_obj_set_style_pad_all(row, 2, 0);
      lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
      lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START,
                            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

      /* Checkbox (done/undone) */
      lv_obj_t *cb = lv_checkbox_create(row);
      lv_checkbox_set_text(cb, "");
      lv_obj_set_style_text_color(cb,
        today_tasks[i].done ? lv_color_hex(0x00ff88) : lv_color_hex(0x888888), 0);

      if (today_tasks[i].done)
        lv_obj_add_state(cb, LV_STATE_CHECKED);

      int *task_id = (int *)malloc(sizeof(int));
      *task_id = today_tasks[i].id;
      lv_obj_set_user_data(cb, task_id);
      lv_obj_add_event_cb(cb, checkbox_cb, LV_EVENT_VALUE_CHANGED, NULL);

      /* Subject badge */
      lv_obj_t *subj_lbl = lv_label_create(row);
      lv_label_set_text(subj_lbl, today_tasks[i].subject);
      lv_obj_set_style_text_color(subj_lbl, lv_color_hex(0x533483), 0);

      /* Task content */
      lv_obj_t *content_lbl = lv_label_create(row);
      lv_label_set_text(content_lbl, today_tasks[i].content);
      lv_obj_set_style_text_color(content_lbl,
        today_tasks[i].done ? lv_color_hex(0x666666) : lv_color_hex(0xffffff), 0);
      lv_label_set_long_mode(content_lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
      lv_obj_set_width(content_lbl, 100);

      /* Delete button */
      lv_obj_t *del_btn = lv_btn_create(row);
      lv_obj_set_size(del_btn, 26, 24);
      lv_obj_set_style_bg_color(del_btn, lv_color_hex(0xaa3333), 0);

      lv_obj_t *d_lbl = lv_label_create(del_btn);
      lv_label_set_text(d_lbl, "✕");
      lv_obj_center(d_lbl);
      lv_obj_set_style_text_color(d_lbl, lv_color_hex(0xffffff), 0);

      int *del_id = (int *)malloc(sizeof(int));
      *del_id = today_tasks[i].id;
      lv_obj_set_user_data(del_btn, del_id);
      lv_obj_add_event_cb(del_btn, delete_cb, LV_EVENT_CLICKED, NULL);
    }

  /* Update progress */
  if (g_progress_label)
    {
      char buf[64];
      snprintf(buf, sizeof(buf), "完成: %d / %d", done_count, count);
      lv_label_set_text(g_progress_label, buf);
    }
}

/* Checkbox callback */
static void checkbox_cb(lv_event_t *e)
{
  lv_obj_t *cb = lv_event_get_target(e);
  int *id = (int *)lv_obj_get_user_data(cb);
  if (id)
    {
      bool checked = lv_obj_get_state(cb) & LV_STATE_CHECKED;
      plan_set_done(*id, checked);
      rebuild_list();
    }
}

/* Delete callback */
static void delete_cb(lv_event_t *e)
{
  lv_obj_t *btn = lv_event_get_target(e);
  int *id = (int *)lv_obj_get_user_data(btn);
  if (id)
    {
      plan_remove_task(*id);
      free(id);
      rebuild_list();
    }
}

/****************************************************************************
 * Add task dialog
 ****************************************************************************/

static void dialog_confirm_cb(lv_event_t *e)
{
  if (g_dialog)
    {
      lv_obj_del(g_dialog);
      g_dialog = NULL;
    }

  /* Get selected subject */
  char subj_buf[32];
  lv_dropdown_get_selected_str(g_subj_dd, subj_buf, sizeof(subj_buf));

  /* Get content text */
  const char *content = lv_textarea_get_text(g_content_ta);
  if (strlen(content) == 0) content = "自习";

  plan_add_task(subj_buf, content);
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
  if (g_dialog) return;

  g_dialog = lv_obj_create(lv_scr_act());
  lv_obj_set_size(g_dialog, 220, 180);
  lv_obj_center(g_dialog);
  lv_obj_set_style_bg_color(g_dialog, lv_color_hex(0x0f3460), 0);
  lv_obj_set_style_border_width(g_dialog, 2, 0);
  lv_obj_set_style_border_color(g_dialog, lv_color_hex(0x533483), 0);

  lv_obj_t *title = lv_label_create(g_dialog);
  lv_label_set_text(title, "添加新任务");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Subject dropdown */
  g_subj_dd = lv_dropdown_create(g_dialog);
  lv_dropdown_set_options(g_subj_dd, "政治\n英语\n数学\n专业课\n自定义");
  lv_obj_set_size(g_subj_dd, 120, 30);
  lv_obj_align(g_subj_dd, LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_text_color(g_subj_dd, lv_color_hex(0xffffff), 0);

  /* Content textarea */
  g_content_ta = lv_textarea_create(g_dialog);
  lv_textarea_set_placeholder_text(g_content_ta, "任务内容");
  lv_obj_set_size(g_content_ta, 180, 30);
  lv_obj_align(g_content_ta, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(g_content_ta, lv_color_hex(0xffffff), 0);

  /* Confirm button */
  lv_obj_t *confirm = lv_btn_create(g_dialog);
  lv_obj_set_size(confirm, 70, 28);
  lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, -40, -10);
  lv_obj_set_style_bg_color(confirm, lv_color_hex(0x00aa44), 0);
  lv_obj_add_event_cb(confirm, dialog_confirm_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *c_lbl = lv_label_create(confirm);
  lv_label_set_text(c_lbl, "确认");
  lv_obj_center(c_lbl);

  /* Cancel button */
  lv_obj_t *cancel = lv_btn_create(g_dialog);
  lv_obj_set_size(cancel, 70, 28);
  lv_obj_align(cancel, LV_ALIGN_BOTTOM_MID, 40, -10);
  lv_obj_set_style_bg_color(cancel, lv_color_hex(0x666666), 0);
  lv_obj_add_event_cb(cancel, dialog_cancel_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *x_lbl = lv_label_create(cancel);
  lv_label_set_text(x_lbl, "取消");
  lv_obj_center(x_lbl);
}

/****************************************************************************
 * Create the study plan screen
 ****************************************************************************/

lv_obj_t *ui_study_plan_create(void)
{
  g_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(g_scr, lv_color_hex(0x1a1a2e), 0);

  /* Title */
  lv_obj_t *title = lv_label_create(g_scr);
  lv_label_set_text(title, "复习计划");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Progress summary */
  g_progress_label = lv_label_create(g_scr);
  lv_label_set_text(g_progress_label, "今日暂无任务");
  lv_obj_align(g_progress_label, LV_ALIGN_TOP_MID, 0, 25);
  lv_obj_set_style_text_color(g_progress_label, lv_color_hex(0x00ff88), 0);

  /* Empty state */
  g_empty_label = lv_label_create(g_scr);
  lv_label_set_text(g_empty_label, "今日暂无任务\n点击下方 + 添加");
  lv_obj_align(g_empty_label, LV_ALIGN_CENTER, 0, 10);
  lv_obj_set_style_text_color(g_empty_label, lv_color_hex(0x888888), 0);
  lv_obj_set_style_text_align(g_empty_label, LV_TEXT_ALIGN_CENTER, 0);

  /* Scrollable task list */
  g_container = lv_obj_create(g_scr);
  lv_obj_set_size(g_container, 230, 170);
  lv_obj_align(g_container, LV_ALIGN_CENTER, 0, 5);
  lv_obj_set_style_bg_color(g_container, lv_color_hex(0x1a1a2e), 0);
  lv_obj_set_style_border_width(g_container, 0, 0);
  lv_obj_set_flex_flow(g_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(g_container, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_scrollbar_mode(g_container, LV_SCROLLBAR_MODE_AUTO);

  /* Add button */
  lv_obj_t *add_btn = lv_btn_create(g_scr);
  lv_obj_set_size(add_btn, 50, 30);
  lv_obj_align(add_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_bg_color(add_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(add_btn, add_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *add_lbl = lv_label_create(add_btn);
  lv_label_set_text(add_lbl, "＋ 添加");
  lv_obj_center(add_lbl);
  lv_obj_set_style_text_color(add_lbl, lv_color_hex(0xffffff), 0);

  /* Build initial list */
  rebuild_list();

  return g_scr;
}

/****************************************************************************
 * Public API
 ****************************************************************************/

void ui_study_plan_add_task(const char *task, bool done)
{
  /* Add a task with "自习" subject as default */
  plan_add_task("自习", task);
  rebuild_list();
}

void ui_study_plan_clear(void)
{
  /* Clear all tasks */
  while (plan_get_task_count() > 0)
    {
      plan_task_t t;
      if (plan_get_task(0, &t) == 0)
        plan_remove_task(t.id);
    }
  rebuild_list();
}