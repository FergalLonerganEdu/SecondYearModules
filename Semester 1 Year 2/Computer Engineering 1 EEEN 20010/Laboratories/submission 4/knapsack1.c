/*======================================================================================*
 *                                                                                      *
 *                        University College Dublin                                     *
 *                    EEEN20010 - Computer Engineering I                                *
 *                                                                                      *
 * File Name:   subpack.c                                                               *
 *                                                                                      *
 * Description:  Write a program (subpack.c) which evaluates all the possible packing   *
 *               possibilities, and exhaustively finds the best solution.               *
 *                                                                                      *
 * Author:              Fergal Lonergan  (13456938)                                     *
 * Date of lab:         12/11/2014  (Wednesday Group)                                   *
 * Deadline:            5:00 PM, Monday the 24th of November, 2014.                     *
 *                                                                                      *
 * Aim:         To have the program return the correct result as quickly as possible.   *
 *              For my program I used the greedy method which takes the items of        *
 *              highest value and checks to see if when added to the knapsack their     *
 *              weights or costs break the max allowances. If so it removes the last    *
 *              added item and tries to see if it can obtain a higher value with the    *
 *              items of less value below.                                              *
 *                                                                                      *
 * Approach:    Greedy method
 *              Rough notes about the Algorithm - as implemented in the code above:

                Firstly, input the file name that you would wish to read the values
                from. Then input the maximum weight (maxWeight) and max cost (maxCost).
                Lastly calculate the maximum value that can be attained using
                Knapsack function.

                Knapsack function – This function takes total number of items (items),
                value of all the items (value), weight of all the items (weight),
                the maximum weight (maxWeight), costs of all the items (cost) and
                the max cost (maxCost) as arguments. It returns the maximum value that
                can be attained.

                Declare max_v[items+1][maxWeight+1]. Where, max_v[i][w] represents
                maximum value that can be attained if the maximum weight is w and items
                are chosen from 1...i. max_v[0][w] = 0 for all w because we have chosen
                0 items. And, max_v[i][0] = 0 for all w because maximum weight we can
                take is 0.
Recurrence:
for i=1 to items
    for w=0 to maxWeight
        max_v[i][w] = max_v[i-1][w], if we do not take item i. if w-weight[i] >=0, suppose we take this
        item then, max_v[i][w] = max(max_v[i][w] , max_v[i-1][w-weight[i]]+value[i]). Where, max is a
        function that returns the maximum of the two arguments it takes.
     next
next
Return max_v[items][maxWeight]

 *======================================================================================*/


#include<stdio.h>   /* pre processor directives */
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
        const char * name;
        int value, weight, cost;
} items; /* type defining my structure for items */

items item[] = {
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


int max(int ,int );
int Knapsack(int ,int [],int [],int [],int ,int );

int main()
{
        int items;
        printf("Enter the file name: ");
        scanf("%d",&items);
        int weight[items+1],value[items+1],cost[items+1];
        int i;
        for(i=1;i<=items;i++)
        {
                scanf("%d %d %d",&value[i],&weight[i],&cost[i]);
        }
        int maxWeight=13;
        int maxCost=70;
        printf("Max value attained can be:%d for Max Weight of %d\n",Knapsack(items,weight,value,cost,maxWeight,maxCost),maxWeight,maxCost);
}

int max(int a,int b)
{
        return a>b?a:b;
}
int Knapsack(int items,int weight[],int value[],int cost[],int maxWeight,int maxCost)
{
        int max_v[items+1][maxWeight+1];
        /* max_v[i][w] represents maximum value that can be attained if the maximum weight is w and
           items are chosen from 1...i */
        /* max_v[0][w] = 0 for all w because we have chosen 0 items */
        int V_c[items+1][maxCost+1];
        int i,w,c,n_items=0,max_veight=0,v_cost=0;
        for(i=0;i<=maxWeight;i++)
        {
                max_v[0][i]=0;
        }
        /* max_v[i][0] = 0 for all w because maximum weight we can take is 0 */
        for(i=0;i<=items;i++)
        {
                max_v[i][0]=0;
                V_c[i][0]=0;
        }
        for(i=1;i<=items;i++)
        {
                for(w=0;w<=maxWeight,c<=maxCost;w++,c++)
                {
                        max_v[i][w] = max_v[i-1][w]; /* If I do not take this item */
                        if(w-weight[i] >=0)
                        {
                                if(c-cost[i] >=0)
                                {
                                    /* suppose if I take this item */
                                max_v[i][w] = max(max_v[i][w] , max_v[i-1][w-weight[i]]+value[i]);
                                }
                        }
                }

        }
        printf("No. of items is %d\n",n_items);
        printf("Weight of bag is %d\n",max_veight);
        printf ("Cost of bag is %d\n",v_cost);
        return max_v[items][maxWeight];
}
