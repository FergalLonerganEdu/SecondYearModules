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

#include <stdio.h>
#include <math.h>  /* including libraries of c functions*/

int main()  /* declaration of main function */
{

    int a=1,b=2,c=1,d=0,count=2,n=0;

    printf ("Enter the target number of divisors: ");
    scanf ("%d", &d);
        while (a<100001)
        {
            while (a>=(2*b))
            {

                if (a%b==0)
                {
                count ++;
                }
                if (count>d)
                {
                    break;
                }
                b++;
            }

            if (count==d)
                {
                    n++;
                }
            a++;
            count = 2 ;
            b = 2;


        }

    printf("\nThere are %d numbers between 1 and 100000 inclusive which have exactly %d divisors.", n, d);
    return 0; /* end of main */
}
