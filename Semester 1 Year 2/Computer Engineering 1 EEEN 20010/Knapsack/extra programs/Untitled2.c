#include<stdio.h>
int max(int a,int b)
{
        return a>b?a:b;
}
int Knapsack(int items,int weight[],int value[],int maxWeight)
{
        int dp[items+1][maxWeight+1];
        /* dp[i][w] represents maximum value that can be attained if the maximum weight is w and
           items are chosen from 1...i */
        /* dp[0][w] = 0 for all w because we have chosen 0 items */
        int iter,w;
        for(iter=0;iter<=maxWeight;iter++)
        {
                dp[0][iter]=0;
        }
        /* dp[i][0] = 0 for all w because maximum weight we can take is 0 */
        for(iter=0;iter<=items;iter++)
        {
                dp[iter][0]=0;
        }
        for(iter=1;iter<=items;iter++)
        {
                for(w=0;w<=maxWeight;w++)
                {
                        dp[iter][w] = dp[iter-1][w]; /* If I do not take this item */
                        if(w-weight[iter] >=0)
                        {
                                /* suppose if I take this item */
                                dp[iter][w] = max(dp[iter][w] , dp[iter-1][w-weight[iter]]+value[iter]);
                        }
                }

        }
        return dp[items][maxWeight];
}
int main()
{
        int items;
        scanf("%d",&items);
        int weight[items+1],value[items+1];
        int iter;
        for(iter=1;iter<=items;iter++)
        {
                scanf("%d%d",&weight[iter],&value[iter]);
        }
        int maxWeight;
        scanf("%d",&maxWeight);
        printf("Max value attained can be %d\n",Knapsack(items,weight,value,maxWeight));
}

/*Rough notes about the Algorithm - as implemented in the code above:

Firstly, input the total number of items, the weight and value of each item. Then input the
maximum weight (maxWeight). Lastly calculate the maximum value that can be attained using
Knapsack function.

Knapsack function – This function takes total number of items (items), weight of all the items
(weight), value of all the items (value) and the maximum weight (maxWeight) as arguments. It
returns the maximum value that can be attained.

Declare dp[items+1][maxWeight+1]. Where, dp[i][w] represents maximum value that can be attained if the maximum weight is w and items are chosen from 1...i.
dp[0][w] = 0 for all w because we have chosen 0 items. And, dp[i][0] = 0 for all w because maximum weight we can take is 0.
Recurrence:
for i=1 to items
    for w=0 to maxWeight
        dp[i][w] = dp[i-1][w], if we do not tale item i. if w-weight[i] >=0, suppose we take this
        item then, dp[i][w] = max(dp[i][w] , dp[i-1][w-weight[i]]+value[i]). Where, max is a
        function that returns the maximum of the two arguments it takes.
     next
next
Return dp[items][maxWeight]*/
