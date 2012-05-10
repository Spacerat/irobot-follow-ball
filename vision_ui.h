#ifndef _vision_ui_h_
#define _vision_ui_h_

#ifdef __cplusplus
extern "C" {
#endif

void vision_ui_init(int argc, char ** argv);
void vision_ui_run();
void vision_ui_quit();
void vision_ui_lock_image();
void vision_ui_unlock_image();
void vision_ui_update_values(int xpos, int area, int l_speed, int r_speed, float distance, int ballfound);
void vision_ui_get_values(int * xpos, int * area, int * l_speed, int * r_speed, float * distance, int * ballfound);

#ifdef __cplusplus
}
#endif

#endif
