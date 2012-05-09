#ifndef _vision_ui_h_
#define _vision_ui_h_

#ifdef __cplusplus
extern "C" {
#endif

void vision_ui_init(int argc, char ** argv);
void * vision_ui_thread_func(void * ptr);
void vision_ui_quit();

#ifdef __cplusplus
}
#endif

#endif
