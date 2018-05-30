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



 /* including libraries of c functions*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define capacity 13

void knapsack(int , float[], float [], float [] );
/* declaration of main function */

int main()
{
   float value [20], weight[20], cost[20] ;
   int num, i, j;
   float ratio[20], temp;

   printf("\nEnter the no. of objects:- ");
   scanf("%d", &num);

   printf("Enter the values, weights and profits of each object:-\n");
   for (i = 0; i < num; i++) {
      scanf("%f %f %f",&value[i], &weight[i], &cost[i]);
   }

   printf("\nThe max capacity of knapsack:- 13kg");

   for (i = 0; i < num; i++) {
      ratio[i] = value[i]*(cost[i] / weight[i]);
   }

   for (i = 0; i < num; i++) {
      for (j = i + 1; j < num; j++) {
         if (ratio[i] < ratio[j]) {
            temp = ratio[j];
            ratio[j] = ratio[i];
            ratio[i] = temp;

            temp = weight[j];
            weight[j] = weight[i];
            weight[i] = temp;

            temp = cost[j];
            cost[j] = cost[i];
            cost[i] = temp;
         }
      }
   }

   knapsack(num, value, weight, cost );
   return(0);
}





void knapsack(int n,float value[], float weight[], float cost[] ) {
   float x[20], tp = 0;
   int i, j, u;
   u = capacity;

   for (i = 0; i < n; i++)
      x[i] = 0.0;

   for (i = 0; i < n; i++) {
      if (weight[i] > u)
         break;
      else {
         x[i] = 1.0;
         tp = tp + cost[i];
         u = u - weight[i];
      }
   }

   if (i < n)
      x[i] = u / weight[i];

   tp = tp + (x[i] * cost[i]);

  /*printf("\nThe best result calculated is:- ");
   for (i = 0; i < n; i++)
      printf("%f\t", x[i]);*/

   printf("\nThe Best result calculated is:- %f", tp);

}
