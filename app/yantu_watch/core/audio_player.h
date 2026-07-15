#ifndef __YANTU_CORE_AUDIO_PLAYER_H
#define __YANTU_CORE_AUDIO_PLAYER_H

#include <stdbool.h>

typedef struct {
    char title[128];
    char filename[256];
    int  duration_sec;
} audio_track_t;

int  audio_init(void);
int  audio_play(const char *filename);
int  audio_pause(void);
int  audio_resume(void);
int  audio_stop(void);
int  audio_seek(int seconds);
int  audio_get_position(void);
int  audio_get_duration(void);
bool audio_is_playing(void);
int  audio_set_volume(int volume);
int  audio_get_volume(void);
int  audio_next_track(void);
int  audio_prev_track(void);
int  audio_scan_files(void);
int  audio_get_track_count(void);
int  audio_get_track_info(int index, audio_track_t *track);

#endif /* __YANTU_CORE_AUDIO_PLAYER_H */