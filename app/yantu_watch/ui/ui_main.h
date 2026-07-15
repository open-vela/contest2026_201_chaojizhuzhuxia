#ifndef __YANTU_UI_MAIN_H
#define __YANTU_UI_MAIN_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_main_create(void);
void ui_main_update_time(void);
void ui_main_update_study_today(int minutes);
void ui_main_update_countdown_days(int days);

#endif /* __YANTU_UI_MAIN_H */