#ifndef __YANTU_CORE_STUDY_RECORD_H
#define __YANTU_CORE_STUDY_RECORD_H

int  study_record_init(void);
int  study_record_add_minutes(int minutes);
int  study_record_get_today_minutes(void);
int  study_record_get_week_data(int *data, int *count);
int  study_record_get_month_data(int *data, int *count);
int  study_record_get_total_days(void);
int  study_record_get_total_minutes(void);

#endif /* __YANTU_CORE_STUDY_RECORD_H */