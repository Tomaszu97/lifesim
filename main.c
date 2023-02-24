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
    gal_draw_pixel(e->p[0], e->p[1], e->c.r, e->c.g, e->c.b, 255);
    if (e->t == ANIMAL) {
        const animal_t *a = (const animal_t *) e;
        gal_draw_circle(e->p[0], e->p[1], a->los, e->c.r, e->c.g, e->c.b, 255);
        gal_draw_circle(e->p[0], e->p[1], a->lor, e->c.r, e->c.g, e->c.b, 255);
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

    vec3 dir;
    if ((rand() % 100) < a->det) {
        glm_vec3_copy(a->dir, dir);
    }
    else {
        rand_vec_norm(dir);
    }

    glm_vec3_scale(dir, a->spd, dir);
    glm_vec3_add(a->e.p, dir, a->e.p);

    if (a->e.p[0] < 0) {
        a->e.p[0] = 0;
        a->dir[0] *= -1;
    }
    if (a->e.p[0] > WINDOW_WIDTH) {
        a->e.p[0] = WINDOW_WIDTH;
        a->dir[0] *= -1;
    }
    if (a->e.p[1] < 0) {
        a->e.p[1] = 0;
        a->dir[1] *= -1;
    }
    if (a->e.p[1] > WINDOW_HEIGHT) {
        a->e.p[1] = WINDOW_HEIGHT;
        a->dir[1] *= -1;
    }
}

void
animal_eat(animal_t *a)
{
    if (a->s != EATING) return;

    a->f++;
    a->hp += 1;
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

    /* if (g_animals_len > 0)*/
    if (a == &g_animals[0])
        animal_print(&g_animals[0]);

    if (a->s != EATING) {
        a->f--;
        if (a->f < 0) a->f = 0;
    }

    if (a->f == 0) a->hp -= 0.7;
    else if (a->f > 99) a->hp -= 0.4;
    if (a->hp < 0) a->hp = 0;

    if (a->hp <= 0) animal_set_state(a, DEAD);
}

void
animal_look_for_food(animal_t *a)
{
    if (a->s != WANDERING) return;
    if (a->f >= 100) return;

    int i;
    for (i=0; i<g_foods_len; i++) {
        food_t *f = &g_foods[i];
        float dist = hypot(a->e.p[0] - f->e.p[0], a->e.p[1] - f->e.p[1]);
        if (dist < a->los) {
            glm_vec3_sub(f->e.p, a->e.p, a->dir);
            glm_vec3_normalize(a->dir);
        }
        if (dist < a->lor) {
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
        float dist = hypot(a->e.p[0] - t->e.p[0], a->e.p[1] - t->e.p[1]);

        if (dist < a->los) {
            glm_vec3_sub(t->e.p, a->e.p, a->dir);
            glm_vec3_normalize(a->dir);
        }
        if (dist < a->lor) {
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
        rand_vec_rng(a->e.p,
                     0,
                     WINDOW_WIDTH,
                     0,
                     WINDOW_HEIGHT,
                     0,
                     0);
        a->hp       = (rand() % 400) + 200;
        a->det      = (rand() % 20) + 55;
        rand_vec_norm(a->dir);
        a->los      = (rand() % 15) + 40;
        a->lor      = (rand() % 2) + 1;
        a->f        = 0;
        a->p        = (rand() % 3) + 1;
        a->spd      = ((float)(rand() % 12) / 10) + 0.3;
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
        f->e.p[0]   = (rand() % (max_x - min_x)) + min_x;
        f->e.p[1]   = (rand() % (max_y - min_y)) + min_y;
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
        t->e.p[0]   = (rand() % (max_x - min_x)) + min_x;
        t->e.p[1]   = (rand() % (max_y - min_y)) + min_y;
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
                     10);

    foods_rand_gen(0,
                   WINDOW_WIDTH,
                   0,
                   WINDOW_HEIGHT,
                   120);

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

        usleep(1000);
    }

    return 0;
}
