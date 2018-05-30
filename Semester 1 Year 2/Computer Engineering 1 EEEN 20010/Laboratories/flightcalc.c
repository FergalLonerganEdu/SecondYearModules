/* FLIGHT TIME OF AN AEROPLANE (PART1) */
/* WEEK 1 */
/* THIS PROGRAM IS DESIGNED TO READ IN A DISTANCE VALUE ENTERED BY THE USER */
/* AND RETURN THE FLIGHT TIME IN HOURS AND MINUTES */

#include <stdio.h> /* pre-processor directives*/

int main (void) /* main program*/
{

    float time_taken_h;                /* defining variables*/
    float time_taken_r;
    float time_taken_m;
    const int  speed_mph = 529;        /* declaring the constant for speed in mph*/
    float speed_mpm;
    int distance_m;
    int distance_km;
    int hours;
    int mins;

    printf("Enter value travelled by plane :");
    scanf("%d", &distance_km);         /* scanning in distance value from user*/

    distance_m = (0.621371*(distance_km));
    hours = distance_m/speed_mph;
    time_taken_r = distance_m%speed_mph;
    hours = (int)(time_taken_r);
    speed_mpm = speed_mph/60.0;
    time_taken_m = time_taken_r/speed_mpm;
    mins = (int)(time_taken_m);
    /*double num = 23.345;
  int intpart = (int)num;
  double decpart = num - intpart;
  printf("Num = %f, intpart = %d, decpart = %f\n", num, intpart, decpart)*/

    printf ("the flight time is %d hours and %d minutes",hours, mins);


    return (0); /* end of program*/

}
