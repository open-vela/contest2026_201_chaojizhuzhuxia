#ifndef __YANTU_UI_AUDIO_H
#define __YANTU_UI_AUDIO_H

#include <lvgl/lvgl.h>

lv_obj_t *ui_audio_create(void);
void ui_audio_update_title(const char *title);
void ui_audio_update_progress(int current_sec, int total_sec);
void ui_audio_set_playing(bool playing);

#endif /* __YANTU_UI_AUDIO_H */