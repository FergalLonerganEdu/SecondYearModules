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
 * Deadline:            5:00 PM, Monday the 27th of October, 1969.                      *
 *                                                                                      *
 * Aim:         To have the program return the correct results as quickly as possible.  *
 *                                                                                      *
 * Approach: 1) To have a running loop where a, (our incrementing number), is divided   *
 *              by b (our test divisor to find if b is a divisor or not within our      *
 *              range of 100000.                                                        *
 *           2) To have a loop where b increments up to the square root which is the    *
 *              median divisor. for every divisor b we increment by two as every        *
 *              that is not a square root has a pair.                                   *
 *           3) To - one from our count if our number is a perfect square to account    *
 *              for the fact we count it twice earlier in the program.                  *
 *           4) To increment n (our amount of numbers with d divisors) every time we    *
 *              we find a number with d divisors.                                       *
 *           5) To print out our results to the user.                                   *
 *           6) To make special cases for 1, 0 and any negative number entered by the   *
 *              user.                                                                   *
 *                                                                                      *
 *                                                                                      *
 *======================================================================================*/



 /* NOTE: FUNCTIONS WERE NOT USED AS THEY SLOWED DOWN THE RUN TIME. */




 /* including libraries of c functions*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define READONLY "r";
#define WRITEONLY "w";

/* declaration of main function */
int main()
{


FILE *fopen(char *filename, char *mode);

char fopn[10];
printf("Give the filename with the unknown word: ");
scanf("%s", &fopn);
FILE *fpin;

 if((fpin = fopen(“input.dat”, “r”)) == NULL){
 printf(“Cannot open file\n”);
 exit(EXIT_FAILURE);
 }

/* end of main */
    return 0;
}

