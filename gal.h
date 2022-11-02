#ifndef GAL_H
#define GAL_H
/* graphics abstraction layer */

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define ANTIALIASING_LEVEL 1
#define WINDOW_SCALE 2

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
unsigned int last_ticks;

void
gal_init(const char *window_title);

void
gal_stop();

unsigned int
gal_get_last_ticks();

void
gal_set_bgcolor(const unsigned int r,
                const unsigned int g,
                const unsigned int b);

void
gal_draw_pixel(const unsigned int x,
               const unsigned int y,
               const unsigned int r,
               const unsigned int g,
               const unsigned int b,
               const unsigned int a);

void
gal_draw_line(const unsigned int x1,
              const unsigned int y1,
              const unsigned int x2,
              const unsigned int y2,
              const unsigned int r,
              const unsigned int g,
              const unsigned int b,
              const unsigned int a);

void
gal_draw_circle(const unsigned int x,
                const unsigned int y,
                const unsigned int rad,
                const unsigned int r,
                const unsigned int g,
                const unsigned int b,
                const unsigned int a);

void
gal_draw_fill_circle(const unsigned int x,
                     const unsigned int y,
                     const unsigned int rad,
                     const unsigned int r,
                     const unsigned int g,
                     const unsigned int b,
                     const unsigned int a);

void
gal_clear_screen();

void
gal_flip_screen();

void
gal_handle_events();

#endif /* GAL_H */
