/*======================================================================================*
 *                                                                                      *
 *                        University College Dublin                                     *
 *                    EEEN20010 - Computer Engineering I                                *
 *                                                                                      *
 * File Name:   divisors.c                                                              *
 *                                                                                      *
 * Description:  In the game of hangman, one player picks a word, and the other player
 * has to try to guess what the word is by selecting one letter at a time. If they select
 a correct letter, all occurrences of the letter are shown. If no letter shows up, they
 use up one of their turns. The player is allowed to use no more than 10 incorrect
 turns to guess what the word is. They can guess the word at any time, but if they
 guess incorrectly, they lose a turn.

To simulate this game on a computer, the unknown word is stored in a simple text file
(which only stores one word). The computer will display the word but with asterisks in
place of letters. This lets the player know how many letters are in the word. The
player is then asked to guess one of the 26 letters, or to guess what the word is.
If they choose a letter, an updated version of the word is shown with that letter
filled in, and their number of remaining turns remains fixed. If they make an
incorrect guess, they lose a turn                                                        *
 *                                                                                      *
 * Author:              Fergal Lonergan  (13456938)                                     *
 * Date of lab:         15/10/2014  (Wednesday Group)                                   *
 * Deadline:            5:00 PM, Monday the 10th of November, 2014.                      *
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

char word[] = "elephant";
char a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
int lives=10;
char letter_entered;


/* declaration of main function */
int main()
{
    for(lives=10;lives>=0;lives--)
    {
        printf ("you have ten goes. enter a letter. ");
        scanf ("%c", &letter_entered);
        for (i=0;i<8;i++)
        {
            if (word[i] = letter_entered)
            {
                printf ("y");
            }
            else
            {
                printf ("n");
            }
        }
    }

/* end of main */
    return 0;
}

