/*======================================================================================*
 *                                                                                      *
 *                        University College Dublin                                     *
 *                    EEEN20010 - Computer Engineering I                                *
 *                                                                                      *
 * File Name:   divisors.c                                                              *
 *                                                                                      *
 * Description:  Write a program (divisors.c) which reads in an integer k, and prints   *
 * out the number of positive integers between 1 and 100000 (inclusive) which have      *
 * exactly k divisors. As an example, the number 24 has 8 divisors:                     *
 * 1, 2, 3, 4, 6, 8, 12, and 24.                                                        *
 *                                                                                      *
 * Author:              Fergal Lonergan  (13456938)                                     *
 * Date of lab:         15/10/2014  (Wednesday Group)                                   *
 * Deadline:            5:00 PM, Monday the 27th of October, 2014.                      *
 *                                                                                      *
 *======================================================================================*/
#include <time.h>
#include <stdio.h>
#include <math.h>  /* including libraries of c functions*/

int main()  /* declaration of main function */
{

    int a,b,d,count,n=0; /*declaration of variables */

    printf ("Enter the target number of divisors: "); /* direction to user */
    scanf ("%d", &d);                                 /* reading in value of divisors */
    clock_t time = clock();
    if (d>1)                                          /* seeing as 1 is always a divisor we treat it as a special case */
    {
          for (a=2;a<100001;a++)                      /* running through all the numbers from 2 to 100000. 1 is a special case */
        {
            count=2; /* resetting count to = 2 to account for 1 and the number a being divisors */
            for(b=2;b<=sqrt(a);b++) /* starting dividing by two as 1 is always a divisor */
            {
                /* as long as the remainder is 0 then its a divisor. add 2 to account for both divisors instead of calculating both twice */
                if(a%b==0)
                {
                    count +=2;
                }
                 if(count>(d+1)) /* once the count of divisors exceeds our specified divisors exit the loop. waste of time calculating */
                {
                    break;
                }
            }

            if(sqrt(a) == b) /* if the number is a perfect square take one from count as we counted two for it above */
            {
                count--;
            }

            if(count==d) /* once the count equals our target divisor then add 1 to our number */
            {
                n++;
            }

        printf("a=%d count=%d\n",a,count);
        }
    }
    else                                         /* if d=1 then then there is only one number with 1 divisor. 1. */
    {
        n=1;
    }


    printf("\nThere are %d numbers between 1 and 100000 inclusive which have exactly %d divisors.", n, d);
    /* print out the amount of numbers with the specified divisors */
    printf("\nTime=%f",(clock()-time)/(float)CLOCKS_PER_SEC);
    return 0; /* end of main */
}
