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

#define max_w 13

typedef struct {
        const char * name;
        int value, weight, cost;
} item_t;

item_t item[] = {
        {"a",                   7,       1,         4},
        {"b",                   6,       5,         10},
        {"c",                   6,       11,        15},
        {"d",                   5,       4,         8},
        {"e",                   4,       2,         20},
        {"f",                   3,       6,         8},
        {"g",                   2,       4,         9},
        {"h",                   2,       10,        3},
        {"i",                   1,       6,         7},
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
        optimal(max_w, n_items - 1, &s);

        for (i = 0; i < n_items; i++) {
                if (s.bits & (1 << i)) {
                        printf("%s\n", item[i].name);
                        w += item[i].weight;
                }
        }
        printf("Total value: %d; weight: %d; max weight:%d\n", s.value, w, max_w);
        return 0;
}
