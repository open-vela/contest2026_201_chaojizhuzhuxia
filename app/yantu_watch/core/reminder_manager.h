#ifndef __YANTU_CORE_REMINDER_MANAGER_H
#define __YANTU_CORE_REMINDER_MANAGER_H

#include <stdbool.h>

#define REMINDER_MAX_ITEMS 16

typedef struct {
    int  id;
    char label[64];
    int  hour;
    int  minute;
    bool repeat;       /* repeat daily */
    bool enabled;
    bool vibrate;      /* silent vibration */
} reminder_item_t;

int  reminder_init(void);
int  reminder_add(const char *label, int hour, int minute, bool repeat);
int  reminder_remove(int id);
int  reminder_update(int id, const reminder_item_t *item);
int  reminder_get_count(void);
int  reminder_get_item(int index, reminder_item_t *item);
int  reminder_set_enabled(int id, bool enabled);
int  reminder_get_item_by_id(int id, reminder_item_t *item);
void reminder_check_alarm(void);

#endif /* __YANTU_CORE_REMINDER_MANAGER_H */