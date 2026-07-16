#ifndef __YANTU_UI_VOCABULARY_H
#define __YANTU_UI_VOCABULARY_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_vocabulary_create(void);
void ui_vocabulary_refresh(void);
void ui_vocabulary_update_progress(int current, int total);

#endif /* __YANTU_UI_VOCABULARY_H */