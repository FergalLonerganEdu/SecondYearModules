/*======================================================================================*
 *                                                                                      *
 *                        University College Dublin                                     *
 *                    EEEN20010 - Computer Engineering I                                *
 *                                                                                      *
 * File Name:   optpack.c                                                               *
 *                                                                                      *
 * Description:  Write a program (optpack.c) which evaluates all the possible packing   *
 *               possibilities, and exhaustively finds the best maxValue.               *
 *                                                                                      *
 * Author:              Fergal Lonergan  (13456938)                                     *
 * Date of lab:         12/11/2014  (Wednesday Group)                                   *
 * Deadline:            5:00 PM, Monday the 24th of November, 2014.                     *
 *                                                                                      *
 * Aim:         To have the program return the correct results after running through    *
 *              all combinations possible. It will then always give the correct,        *
                optimum result.                                                         *
 *                                                                                      *
 * Approach:    First the program asks for the file and reads it in. It then creates an
                array of structs of type item parameters from the data read for file.
                It then asks for the user to imput the max cost and weight of the
                knapsack which will be used as boundary conditions later on in the
                program. We then initialize a struct of maxValue which holds the bits
                values. We then call our function optimal which creates binary
                combinations for each possible packing sequence that we can formulate
                from our items and then returns the optimal pack ie. highest value
                within our weight and cost constraints. finally it prints out the items
                packed the actual value, weight and cost of the knapsack and the max
                weight and cost entered.                                                *
 *                                                                                      *
 *======================================================================================*/


#include <stdio.h>               /* pre processor directives */.
#include <stdlib.h>
#include <stdint.h>

/* Type defining Item_parameters which will be used in the optimal function to group parameters read from each line in the file
into items. Each item has it's own distinctive value, weight and cost. I will then make an array of items*/
typedef struct {
        int value, weight, cost;
} Item_parameters;

/* creating our array of type item */
Item_parameters items[25];


typedef struct {
        int bits, value;
} maxValue;

void optimal(int weight,int cost, int idx, maxValue *s);

/* main function */
int main(void)
{
        /* declaring all my variables */
        FILE* fpin;
        char file_name[50];
        int i = 0, w = 0, c = 0, maxWeight, maxCost, n_items;

        /* managing and reading data from file */
        printf("Enter in name of file:");
        scanf("%s", file_name);
        if((fpin = fopen(file_name, "r")) == NULL)
        {
                printf("Cannot open file\n");   exit(EXIT_FAILURE); /* Tells us if there is an Error reading File and exits program */
        }
        while(fgetc(fpin)!='\n'); /* Declaring a new item for each new line */
        int reading=1; /* This is a condition to check if we have reached the end of file or not */
        for(i=0;reading!=EOF;i++){
            reading=fscanf(fpin,"%d %d %d",&(items[i].value),&(items[i].weight),&(items[i].cost));
        }
        n_items= i-1;

        /* reading in max weight and cost*/
        printf("\nEnter max weight:");
        scanf("%d", &maxWeight);
        printf("\nEnter max cost:");
        scanf("%d", &maxCost);

        /* returning the list of items */
        printf("\nItem list:\n");
        maxValue s = {0, 0};
        optimal(maxWeight,maxCost, n_items - 1, &s); /* calling the functional optimal to find optimal knapsack items */


        /* runs through all the binary options printing the selected items and adding to the weight and cost of knapsack.
        The condition is if our binary number for s.bits and 1 is shifted one to the left.*/
        for (i = 0; i < n_items; i++) {
                if (s.bits & (1 << i)) {
                        printf("%d\n", i+1); /* return knapsack items*/
                        w += items[i].weight;
                        c += items[i].cost;
                }
        }
        /* print the value actual weight and cost and max weight and cost*/
        printf("Total value: %d; weight: %d; cost:%d; max weight:%d ; max cost:%d\n", s.value, w, c, maxWeight, maxCost);
        fclose(fpin); /* close file */
        return 0; /* end of main */
}


void optimal(int weight,int cost, int i, maxValue *s)
{
        /* this runs through all the combinations */
        maxValue v1, v2;
        if (i < 0)
        {
                s->bits = s->value = 0;
                return;
        }

        /* this finds the makes sure max weight isnt broken */
        if (weight < items[i].weight || cost <items[i].cost)
        {
                optimal(weight, cost, i - 1, s);
                return;
        }

        /* calling the function optimal to find our optimum original and new value */
        optimal(weight, cost, i - 1, &v1);
        optimal(weight - items[i].weight, cost - items[i].cost, i - 1, &v2);

        v2.value += items[i].value; /* our new value is equal to the value + the value of the new item */
        v2.bits |= (1 << i);

        *s = (v1.value >= v2.value) ? v1 : v2; /* if our original value is greater than our new value give original else give new*/
}
