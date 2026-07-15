#ifndef __YANTU_CORE_POMODORO_TIMER_H
#define __YANTU_CORE_POMODORO_TIMER_H

#include <stdbool.h>

typedef enum {
    POMODORO_IDLE,
    POMODORO_FOCUS,
    POMODORO_BREAK,
    POMODORO_COMPLETED
} pomodoro_state_t;

typedef enum {
    SUBJECT_POLITICS,
    SUBJECT_ENGLISH,
    SUBJECT_MATH,
    SUBJECT_MAJOR,
    SUBJECT_CUSTOM
} subject_t;

/* Callback for state transitions (for vibration / UI notification) */
typedef void (*pomodoro_callback_t)(pomodoro_state_t state, subject_t subject);

int  pomodoro_init(void);
void pomodoro_register_callback(pomodoro_callback_t cb);
int  pomodoro_start(subject_t subject);
int  pomodoro_pause(void);
int  pomodoro_resume(void);
int  pomodoro_stop(void);
int  pomodoro_skip(void);
int  pomodoro_tick(void); /* Call every second */
pomodoro_state_t pomodoro_get_state(void);
subject_t       pomodoro_get_subject(void);
const char     *pomodoro_get_subject_name(subject_t subject);
int  pomodoro_get_remaining_seconds(void);
int  pomodoro_get_today_count(void);
int  pomodoro_set_duration(int focus_minutes, int break_minutes);
void pomodoro_get_duration(int *focus_minutes, int *break_minutes);

#endif /* __YANTU_CORE_POMODORO_TIMER_H */