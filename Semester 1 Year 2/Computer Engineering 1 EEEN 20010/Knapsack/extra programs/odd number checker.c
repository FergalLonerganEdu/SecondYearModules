#include <stdio.h>

int main (void)
{
    int n,i;
    int sum=1;

    printf("Enter the upper limit:\n");
    scanf("%d",&n);

    sum=n/2;
    if(n%2==0)
        sum=sum;
    else
        sum = sum+1;
   /* for(i=1;i<n;i++)
    {
        if(i%2==0)
            sum ++;
    }*/
    printf("the result is %d", sum);
    return(0);
}
