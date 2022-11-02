#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdlib.h>
#include "colors.h"

typedef enum {
    NONE    = 0,
    UP      = 1,
    DOWN    = 2,
    LEFT    = 3,
    RIGHT   = 4
} direction_e;

typedef enum {
    GENERIC_ENTITY,
    ANIMAL,
    FOOD,
    TOILET
} entity_type_t;

typedef struct {
    int x;
    int y;
} position_t;

typedef struct {
    unsigned int r;
    unsigned int g;
    unsigned int b;
} color_t;

typedef struct {
    entity_type_t t;
    position_t p;
    color_t c;
} entity_t;

typedef enum {
    DEAD,
    RESTING,
    WANDERING,
    EATING,
    IN_TOILET,
    TALKING
} animal_state_t;

typedef struct {
    entity_t e;
    int hp;
    direction_e dir;
    int det;          /* determination - percent */
    int los;          /* line of sight */
    int lor;          /* line of reach */
    animal_state_t s; /* what is animal doing */
    int t;            /* timer counting until end of state */
    int f;            /* fullness (percent) */
    int p;            /* attacking power */
    //position_t memory[2];
} animal_t;

typedef struct {
    entity_t e;
    int cal;
} food_t;

typedef struct {
    entity_t e;
    int c;              /* capacity left (percent) */
} toilet_t;

animal_t g_animals[4096];
size_t g_animals_len = 0;
food_t g_foods[4096];
size_t g_foods_len = 0;
toilet_t g_toilets[4096];
size_t g_toilets_len = 0;

typedef void
void_hdl_ct(const void *p);

typedef void
void_hdl_t(void *p);

void
foreach_c(const void *pl,
          const unsigned int len,
          const unsigned int offset,
          void_hdl_ct *hdl)
{
    unsigned int i;
    for (i=0; i<len; i++) {
        const void *p = pl + (i*offset);
        hdl(p);
    }
}

void
foreach(void *pl,
        const unsigned int len,
        const unsigned int offset,
        void_hdl_t *hdl)
{
    unsigned int i;
    for (i=0; i<len; i++) {
        void *p = pl + (i*offset);
        hdl(p);
    }
}

direction_e
rand_direction()
{
    return (rand() % 4) + 1;
}

void
entity_set_color(entity_t *e,
                 unsigned int r,
                 unsigned int g,
                 unsigned int b)
{
    e->c.r = r;
    e->c.g = g;
    e->c.b = b;
}

void
animal_set_state(animal_t *a,
                 const animal_state_t s)
{
    a->s = s;
    switch (s) {
        case DEAD:
            a->t = 100;
            entity_set_color(&a->e, COL_PURP);
            break;
        case RESTING:
            a->t = 30;
            entity_set_color(&a->e, COL_GRAY);
            break;
        case WANDERING:
            a->t = a->hp * 2;
            entity_set_color(&a->e, COL_WHITE);
            break;
        case EATING:
            /* timer already set  */
            entity_set_color(&a->e, COL_GREEN);
            break;
        case IN_TOILET:
            /* timer already set  */
            entity_set_color(&a->e, COL_PURP2);
            break;
        case TALKING:
            a->t = 30;
            entity_set_color(&a->e, COL_YELLOW);
            break;
    }
}

void
animal_remove(animal_t *a)
{
    int idx = a - g_animals;

    if (g_animals_len == 0) return;

    memcpy(&g_animals[idx], &g_animals[g_animals_len-1], sizeof(animal_t));
    g_animals_len--;

}

#endif /* ENTITIES_H */
