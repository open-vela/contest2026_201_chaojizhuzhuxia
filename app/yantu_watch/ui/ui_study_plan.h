#ifndef __YANTU_UI_STUDY_PLAN_H
#define __YANTU_UI_STUDY_PLAN_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_study_plan_create(void);
void ui_study_plan_add_task(const char *task, bool done);
void ui_study_plan_clear(void);

#endif /* __YANTU_UI_STUDY_PLAN_H */