#ifndef __YANTU_CORE_VIBRATOR_H
#define __YANTU_CORE_VIBRATOR_H

typedef enum {
    VIBRATE_SHORT,   /* 200ms single tap */
    VIBRATE_LONG,    /* 500ms single buzz */
    VIBRATE_DOUBLE,  /* Two short taps */
    VIBRATE_TRIPLE,  /* Three medium buzzes */
    VIBRATE_PULSE,   /* Rapid pulse */
    VIBRATE_MAX
} vibrate_pattern_t;

typedef struct {
    int duty;     /* PWM duty cycle */
    int on_ms;    /* Vibration on time */
    int off_ms;   /* Vibration off time */
    int repeat;   /* Number of repeats */
} vibrate_pattern_def_t;

int vibrator_init(void);
int vibrator_set(vibrate_pattern_t pattern);
int vibrator_off(void);

#endif /* __YANTU_CORE_VIBRATOR_H */