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

*======================================================================================*/


#include<stdio.h>   /* pre processor directives */
#include <stdlib.h>
#include <stdint.h>

/* Used for Boolean (True/False) variables, attributes 0 to false and 1 to true these are the only possible values for boolean.*/
typedef enum {FALSE,TRUE} Boolean;
/* type defining the struct which will hold the value, weight and cost of each item */
typedef struct {
        int value, weight, cost;
} item;
/* declaring an array of structs of type item which is basically our item list*/
item item_list[25];

void bubble_sort(item [], int );
void Knapsack(item *,int ,int ,int);/* declaring my function prototypes */
int packed_values[25];

int main() /* declaring main */
{
        FILE* fpin;
        char file_name[50];
        int i = 0, w = 0, c = 0,value, maxWeight, maxCost, n_items;

        /* managing and reading data from file. user inputs file name. program then reads file and saves info to our array item_list of
        struct item. if it occurs an error it exits the file. it skips the first line as we do not wanmt this data.
        every time the reader meets the new line character it sets up a new item in the array. it also counts the amount of items
        in the file discounting the first line.*/
        printf("Enter in name of file:");
        scanf("%s", file_name);
        if((fpin = fopen(file_name, "r")) == NULL)
        {
                printf("Cannot open file\n");   exit(EXIT_FAILURE);
        }
        while(fgetc(fpin)!='\n');
        int reading=1;
        for(i=0;reading!=EOF;i++){
            reading=fscanf(fpin,"%d %d %d",&(item_list[i].value),&(item_list[i].weight),&(item_list[i].cost));
        }
        n_items = i-1;

        /*reading in the max weight and cost as entered by the user*/
        printf("Enter max weight:");
        scanf("\n%d", &maxWeight);
        printf("Enter max cost:");
        scanf("\n%d", &maxCost);

       /*here we call our bubblesort function which takes our array of items and sorts in accordance to value from
       highest to lowest. we then have our knapsack function which finds the best solution for our knapsack and
       prints the values for the user to see*/
        bubble_sort(item_list, n_items);
        Knapsack(item_list,n_items,maxWeight,maxCost);

        /* here we just close the file we opened and end main */
        fclose(fpin); /*close file*/
        return(0);
}
/* the bubble sort algorithm takes the items array and the no. of items in and sorts it in order of value from highest to lowest.
it does this by checking the first adjacent pair to see if the second value is lower than the first, if it is
it doesnt swap them if it is not then it does swap them. it then moves onto the next adjacent pair. when it
gets to the end it returns to the start and repeats the entire sequence again until all the items are sorted.*/
void bubble_sort(item *item_list, int n_items)
{
     int j, i=1;
     item temp;
	Boolean exchange_made;
	exchange_made = TRUE;

	/* Make up to n - 1 passes through item_list, exit early if no
	exchanges are made on previous pass */
	for (i=1;(i <= n_items) && exchange_made;i++)
	{
		exchange_made = FALSE; /* set our initial condition to be false so if statement must be true before we change values around */
		for (j = 0; j <= n_items - i; ++j)		 /* Number of comparisons on ith pass */
			if (item_list[j].value >= item_list[j + 1].value)
			{
				temp = item_list[j]; 		 /* allowing temp to be the first item in adjacent pair*/
				item_list[j] = item_list[j + 1]; /* then getting the second item in pair */
				item_list[j + 1] = temp; /*moving on to next adjacent pair*/
				exchange_made = TRUE; /* if the second item is larger swap them */
			}
    }

}

/* our knapsack program takes in our array number of items and max weight and cost. it then adds the items of highest value to the knapsack.
if a condition is broken it takes away the last item then checks if a higher value can be obtained from the remaining items. it adds all the
    selected items to an array and we can then use that array to print result to user. */
void Knapsack(item *item_list, int n_items, int maxWeight, int maxCost)
{
    int i, value=0, weight=0, cost=0, count=0;
    /*The loop starts at the highest item in the array which contains the item with the best ratio.*/
    for(i=n_items; i>0; i--)
    {
        value += item_list[i].value;
        weight += item_list[i].weight;
        cost += item_list[i].cost;
        packed_values[count]=i;
        count++;
        /*The count integer is incremented and used later when printing the packing solution.*/
        if(weight > maxWeight || cost > maxCost)
        {
            /*If the limits are reached the most recent item is removed and an item further down the list will attempted to be added on the next loop.*/
            value -= item_list[i].value;
            weight -= item_list[i].weight;
            cost -= item_list[i].cost;
            count--;
        }
    }
    printf("Packed values are: \n\n");
    /*The packing solution obtained is printed when it has been obtained by the loop above.*/
    for(i=0; i<count; i++)
    {
        printf("Value %d Weight %d Cost %d\n", item_list[packed_values[i]].value, item_list[packed_values[i]].weight, item_list[packed_values[i]].cost);
    }
    /* we then print out our desired response of value,weight,cost,max weight and max cost*/
    printf("\nTotal value: %d Total weight: %d Total cost: %d Max weight:%d Max cost:%d", value, weight, cost, maxWeight, maxCost);
}
