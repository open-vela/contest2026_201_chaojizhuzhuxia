#ifndef __YANTU_UI_COUNTDOWN_H
#define __YANTU_UI_COUNTDOWN_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_countdown_create(void);
void ui_countdown_update(int days, int hours, int mins, int secs);

#endif /* __YANTU_UI_COUNTDOWN_H */