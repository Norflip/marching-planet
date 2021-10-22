#ifndef __WINDOW__
#define __WINDOW__

typedef struct Window Window;

void window_initialize(Window* window, int width, int height, const char* title);
void window_run (Window* window, void(*update_fn)(void), void(*render_fn)(void));
void window_close (Window* window);


#endif //__WINDOW__