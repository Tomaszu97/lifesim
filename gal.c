#include "gal.h"

void
gal_init(const char *window_title)
{
    last_ticks = SDL_GetTicks();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);
    SDL_SetWindowTitle(window, window_title);
    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) !=0 ) exit(69);
}

void
gal_stop()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

unsigned int
gal_get_last_ticks()
{
    Uint32 tmp = last_ticks;
    last_ticks = SDL_GetTicks();
    return last_ticks - tmp;
}

void
gal_draw_pixel(const unsigned int x,
               const unsigned int y,
               const unsigned int r,
               const unsigned int g,
               const unsigned int b,
               const unsigned int a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(
        renderer,
        &(SDL_Rect) {
            .x=x*WINDOW_SCALE,
            .y=y*WINDOW_SCALE,
            .w=WINDOW_SCALE,
            .h=WINDOW_SCALE
        });
}

void
gal_draw_line(const unsigned int x1,
              const unsigned int y1,
              const unsigned int x2,
              const unsigned int y2,
              const unsigned int r,
              const unsigned int g,
              const unsigned int b,
              const unsigned int a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x1*WINDOW_SCALE, y1*WINDOW_SCALE, x2*WINDOW_SCALE, y2*WINDOW_SCALE);
}

void
gal_draw_circle(const unsigned int x,
                const unsigned int y,
                const unsigned int rad,
                const unsigned int r,
                const unsigned int g,
                const unsigned int b,
                const unsigned int a)
{
    int offsetx = 0;
    int offsety  = rad;
    int d = rad - 1;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    while (offsety >= offsetx) {
        gal_draw_pixel(x + offsetx, y + offsety, r, g, b, a);
        gal_draw_pixel(x + offsety, y + offsetx, r, g, b, a);
        gal_draw_pixel(x - offsetx, y + offsety, r, g, b, a);
        gal_draw_pixel(x - offsety, y + offsetx, r, g, b, a);
        gal_draw_pixel(x + offsetx, y - offsety, r, g, b, a);
        gal_draw_pixel(x + offsety, y - offsetx, r, g, b, a);
        gal_draw_pixel(x - offsetx, y - offsety, r, g, b, a);
        gal_draw_pixel(x - offsety, y - offsetx, r, g, b, a);

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (rad - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

void
gal_draw_fill_circle(const unsigned int x,
                     const unsigned int y,
                     const unsigned int rad,
                     const unsigned int r,
                     const unsigned int g,
                     const unsigned int b,
                     const unsigned int a)
{
    int offsetx = 0;
    int offsety = rad;
    int d = rad -1;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    while (offsety >= offsetx) {
        gal_draw_line(x - offsety, y + offsetx, x + offsety, y + offsetx, r, g, b, a);
        gal_draw_line(x - offsetx, y + offsety, x + offsetx, y + offsety, r, g, b, a);
        gal_draw_line(x - offsetx, y - offsety, x + offsetx, y - offsety, r, g, b, a);
        gal_draw_line(x - offsety, y - offsetx, x + offsety, y - offsetx, r, g, b, a);

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (rad - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

void
gal_flip_screen()
{
    SDL_RenderPresent(renderer);
}

void
gal_clear_screen()
{
    SDL_RenderClear(renderer);
}

void
gal_set_bgcolor(unsigned int r, unsigned int g, unsigned int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 0);
}

void
gal_handle_events()
{
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gal_stop();
        }
    }
}
