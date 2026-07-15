#ifndef __YANTU_CORE_COUNTDOWN_MANAGER_H
#define __YANTU_CORE_COUNTDOWN_MANAGER_H

#include <stdbool.h>

int  countdown_init(void);
int  countdown_get_days_remaining(void);
void countdown_get_time_remaining(int *days, int *hours, int *mins, int *secs);
void countdown_set_exam_date(int year, int month, int day);
void countdown_get_exam_date(int *year, int *month, int *day);

#endif /* __YANTU_CORE_COUNTDOWN_MANAGER_H */