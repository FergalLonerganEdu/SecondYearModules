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

    int a,b,d,count,n=0;

    printf ("Enter the target number of divisors: ");
    scanf ("%d", &d);
    clock_t time = clock();
    if (d>1)
    {
          for (a=1;a<5001;a++)
        {
            count=2;
            for(b=2;b<=sqrt(a);b++)
            {

                if(a%b==0)
                {
                    count +=2;
                }
                 if(count>d)
                {
                    break;
                }
            }
            if(sqrt(a) == b)
            {
                count--;
            }
            if(count==d)
            {
                n++;
            }
if(count==d)
printf("a=%d count=%d\n",a,count);
        }
    }
    else
    {
        n=1;
    }


    printf("\nThere are %d numbers between 1 and 100000 inclusive which have exactly %d divisors.", n, d);
    printf("\nTime=%f",(clock()-time)/(float)CLOCKS_PER_SEC);
    return 0; /* end of main */
}
