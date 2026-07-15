#ifndef __YANTU_UI_REMINDER_H
#define __YANTU_UI_REMINDER_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_reminder_create(void);
void ui_reminder_add_item(const char *label, int hour, int minute, bool repeat);
void ui_reminder_clear(void);

#endif /* __YANTU_UI_REMINDER_H */