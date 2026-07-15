#ifndef __YANTU_CORE_STUDY_PLAN_MANAGER_H
#define __YANTU_CORE_STUDY_PLAN_MANAGER_H

#include <stdbool.h>

#define PLAN_MAX_TASKS 32

typedef struct {
    int  id;
    char subject[32];
    char content[256];
    bool done;
    int  date; /* YYYYMMDD */
} plan_task_t;

int  plan_init(void);
int  plan_add_task(const char *subject, const char *content);
int  plan_remove_task(int id);
int  plan_set_done(int id, bool done);
int  plan_get_today_tasks(plan_task_t *tasks, int *count);
int  plan_get_task_count(void);
int  plan_get_task(int index, plan_task_t *task);

#endif /* __YANTU_CORE_STUDY_PLAN_MANAGER_H */