/*======================================================================================*
 *                                                                                      *
 *                        University College Dublin                                     *
 *                    EEEN20010 - Computer Engineering I                                *
 *                                                                                      *
 * File Name:   optpack.c                                                               *
 *                                                                                      *
 * Description:  Write a program (optpack.c) which evaluates all the possible packing   *
 *               possibilities, and exhaustively finds the best solution.               *
 *                                                                                      *
 * Author:              Fergal Lonergan  (13456938)                                     *
 * Date of lab:         12/11/2014  (Wednesday Group)                                   *
 * Deadline:            5:00 PM, Monday the 24th of November, 2014.                     *
 *                                                                                      *
 * Aim:         To have the program return the correct results as quickly as possible.  *
 *                                                                                      *
 * Approach:                                                                            *
 *                                                                                      *
 *======================================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define max_weight 13
#define max_cost 45

typedef struct {
        const char * name;
        int value, weight, cost;
} item_t;

item_t item[] = {
        {"knife",                   8,       1,         10},
        {"rope",                    2,       3,          5},
        {"book",                    1,       1,          6},
        {"water",                   6,       5,          4},
        {"tin_of_food",             4,       3,          4},
        {"map",                     5,       1,         15},
        {"tent",                    7,       6,         30},
        {"pillow",                  1,       2,          6},
        {"coat",                    6,       2,          9},
};

#define n_items (sizeof(item)/sizeof(item_t))

typedef struct {
        uint32_t bits; /* 32 bits, can solve up to 32 items */
        int value;
} solution;


void optimal(int weight, int idx, solution *s)
{
        solution v1, v2;
        if (idx < 0) {
                s->bits = s->value = 0;
                return;
        }

        if (weight < item[idx].weight) {
                optimal(weight, idx - 1, s);
                return;
         }

        optimal(weight, idx - 1, &v1);
        optimal(weight - item[idx].weight, idx - 1, &v2);

        v2.value += item[idx].value;
        v2.bits |= (1 << idx);

        *s = (v1.value >= v2.value) ? v1 : v2;
}

int main(void)
{
        int i = 0, w = 0;
        solution s = {0, 0};
        optimal(max_weight && max_cost, n_items - 1, &s);

        for (i = 0; i < n_items; i++) {
                if (s.bits & (1 << i)) {
                        printf("%s\n", item[i].name);
                        w += item[i].weight;
                }
        }
        printf("Total value: %d; weight: %d\n", s.value, w);
        return 0;
}
