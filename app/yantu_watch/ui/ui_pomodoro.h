#ifndef __YANTU_UI_POMODORO_H
#define __YANTU_UI_POMODORO_H

#include <lvgl/lvgl.h>
#include "core/pomodoro_timer.h"

lv_obj_t *ui_pomodoro_create(void);
void ui_pomodoro_update_time(int remaining_secs);
void ui_pomodoro_update_state(int state);
void ui_pomodoro_update_count(int count);
void ui_pomodoro_update_subject(subject_t subject);

#endif /* __YANTU_UI_POMODORO_H */