#ifndef __YANTU_UI_COMPONENTS_H
#define __YANTU_UI_COMPONENTS_H

#include <lvgl/lvgl.h>

/* Create a circular progress bar */
lv_obj_t *ui_comp_circular_progress(lv_obj_t *parent, int size);

/* Create a navigation button */
lv_obj_t *ui_comp_nav_button(lv_obj_t *parent, const char *label,
                             lv_event_cb_t cb);

/* Create a page title */
lv_obj_t *ui_comp_page_title(lv_obj_t *parent, const char *title);

/* Create a card panel */
lv_obj_t *ui_comp_card(lv_obj_t *parent, int w, int h);

#endif /* __YANTU_UI_COMPONENTS_H */