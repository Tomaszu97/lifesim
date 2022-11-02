#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "gal.h"
#include "entities.h"
#include "colors.h"

void
entity_draw(const entity_t *e)
{
    gal_draw_pixel(e->p.x, e->p.y, e->c.r, e->c.g, e->c.b, 255);
    if (e->t == ANIMAL) {
        const animal_t *a = (const animal_t *) e;
        gal_draw_circle(e->p.x, e->p.y, a->los, e->c.r, e->c.g, e->c.b, 128);
        gal_draw_circle(e->p.x, e->p.y, a->lor, e->c.r, e->c.g, e->c.b, 128);
    }
}

void
entities_draw(void)
{
    foreach_c(g_animals,
              g_animals_len,
              sizeof(animal_t),
              (void *)entity_draw);
    foreach_c(g_foods,
              g_foods_len,
              sizeof(food_t),
              (void *)entity_draw);
    foreach_c(g_toilets,
              g_toilets_len,
              sizeof(toilet_t),
              (void *)entity_draw);
}

void
animal_wander(animal_t *a)
{
    if (a->s != WANDERING) return;

    unsigned int dist = /*rand() % 3 +*/ 1;
    direction_e dir;
    if ((rand() % 100) < a->det) {
        dir = a->dir;
    }
    else {
        dir = rand_direction();
    }

    switch (dir) {
        case UP:
            a->e.p.y -= dist;
            break;
        case DOWN:
            a->e.p.y += dist;
            break;
        case LEFT:
            a->e.p.x -= dist;
            break;
        case RIGHT:
            a->e.p.x += dist;
            break;
        case NONE:
            break;
    }

    if (a->e.p.x < 0) {
        a->e.p.x = 0;
        a->dir = RIGHT;
    }
    if (a->e.p.x > WINDOW_WIDTH) {
        a->e.p.x = WINDOW_WIDTH;
        a->dir = LEFT;
    }
    if (a->e.p.y < 0) {
        a->e.p.y = 0;
        a->dir = DOWN;
    }
    if (a->e.p.y > WINDOW_HEIGHT) {
        a->e.p.y = WINDOW_HEIGHT;
        a->dir = UP;
    }
}

void
animal_eat(animal_t *a)
{
    if (a->s != EATING) return;

    a->f++;
    a->hp++;
    if (a->f > 100) a->f = 100;
}

void
animal_sit_on_throne(animal_t *a)
{
    if (a->s != IN_TOILET) return;

    a->f--;
    if (a->f < 0) a->f = 0;
}

void
animal_timeout_transition_state(animal_t *a)
{
    switch (a->s) {
        case RESTING:
        case EATING:
        case TALKING:
        case IN_TOILET:
            animal_set_state(a, WANDERING);
            break;
        case WANDERING:
            animal_set_state(a, RESTING);
            break;
        case DEAD:
            animal_remove(a);
            break;
    }
}

void
animal_decrease_timer(animal_t *a)
{
    a->t--;

    if (a->t < 0) {
        animal_timeout_transition_state(a);
    }
}

void
animal_everyloop(animal_t *a)
{
    /* only for alive animals */
    if (a->s == DEAD) return;

    if (a->hp <= 0) animal_set_state(a, DEAD);

    a->f--;

    if (a->f <= 25) a->hp--;
    else if (a->f >= 75) a->hp--;
}

void
animal_look_for_food(animal_t *a)
{
    if (a->s != WANDERING) return;
    if (a->f >= 100) return;

    int i;
    for (i=0; i<g_foods_len; i++) {
        const food_t *f = &g_foods[i];
        float dist = hypot(a->e.p.x - f->e.p.x, a->e.p.y - f->e.p.y);
        if (dist < a->los) {
            a->t = f->cal;
            animal_set_state(a, EATING);

            memcpy(&g_foods[i], &g_foods[g_foods_len-1], sizeof(food_t));
            g_foods_len--;
        }
    }
}

void
animal_look_for_toilet(animal_t *a)
{
    if (a->s != WANDERING) return;
    if (a->f <= 0) return;

    int i;
    for (i=0; i<g_toilets_len; i++) {

        toilet_t *t = &g_toilets[i];
        float dist = hypot(a->e.p.x - t->e.p.x, a->e.p.y - t->e.p.y);

        if (dist < a->los) {
            int diff = t->c - a->f;
            if (diff >= 0) {
                a->t = a->f;
                t->c -= a->f;
            }
            else {
                a->t = t->c;
                t->c = 0;
            }
            animal_set_state(a, IN_TOILET);
        }
    }
}

void
animals_rand_gen(const unsigned int min_x,
                 const unsigned int max_x,
                 const unsigned int min_y,
                 const unsigned int max_y,
                 const unsigned int cnt)
{
    unsigned int i;
    for (i=0; i<cnt; i++) {
        if (g_animals_len >= sizeof(g_animals)) return;
        animal_t *a = &g_animals[g_animals_len];
        g_animals_len++;

        a->e.t      = ANIMAL;
        a->e.p.x    = (rand() % (max_x - min_x)) + min_x;
        a->e.p.y    = (rand() % (max_y - min_y)) + min_y;
        a->hp       = (rand() % 400) + 20;
        a->det      = (rand() % 40) + 45;
        a->dir      = rand_direction();
        a->los      = (rand() % 6) + 2;
        a->lor      = (rand() % 2) + 1;
        a->f        = 0;
        a->p        = (rand() % 3) + 1;
        animal_set_state(a, WANDERING);
    }
}

void
foods_rand_gen(const unsigned int min_x,
               const unsigned int max_x,
               const unsigned int min_y,
               const unsigned int max_y,
               const unsigned int cnt)
{
    unsigned int i;
    for (i=0; i<cnt; i++) {
        if (g_foods_len >= sizeof(g_foods)) return;
        food_t *f = &g_foods[g_foods_len];
        g_foods_len++;

        f->e.t      = FOOD;
        f->e.p.x    = (rand() % (max_x - min_x)) + min_x;
        f->e.p.y    = (rand() % (max_y - min_y)) + min_y;
        f->cal      = (rand() % 200) + 10;
        entity_set_color(&f->e, COL_RED);
    }
}

void
toilets_rand_gen(const unsigned int min_x,
                 const unsigned int max_x,
                 const unsigned int min_y,
                 const unsigned int max_y,
                 const unsigned int cnt)
{
    unsigned int i;
    for (i=0; i<cnt; i++) {
        if (g_toilets_len >= sizeof(g_toilets)) return;
        toilet_t *t = &g_toilets[g_toilets_len];
        g_toilets_len++;

        t->e.t      = TOILET;
        t->e.p.x    = (rand() % (max_x - min_x)) + min_x;
        t->e.p.y    = (rand() % (max_y - min_y)) + min_y;
        t->c        = (rand() % 30) + 20;
        entity_set_color(&t->e, COL_BROWN);
    }
}
int
main()
{
    srand(123);
    gal_init("simulation");

    animals_rand_gen(0,
                     WINDOW_WIDTH,
                     0,
                     WINDOW_HEIGHT,
                     100);

    foods_rand_gen(0,
                   WINDOW_WIDTH,
                   0,
                   WINDOW_HEIGHT,
                   60);

    toilets_rand_gen(0,
                     WINDOW_WIDTH,
                     0,
                     WINDOW_HEIGHT,
                     40);

    while(1) {
        gal_set_bgcolor(COL_BLACK);
        gal_clear_screen();

        entities_draw();

        /* TODO single loops ? - also check state beforehand */
        /* try to do sth depending on state */
        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_wander);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_eat);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_sit_on_throne);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_look_for_food);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_look_for_toilet);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_decrease_timer);

        foreach(g_animals,
                g_animals_len,
                sizeof(animal_t),
                (void *)animal_everyloop);

        gal_flip_screen();
        gal_handle_events();

        usleep(10000);
    }

    return 0;
}
