typedef struct {
        int value, weight, cost;
} item_t;

int main(void){

 int i;
 int item[50];

 item_t item[] = {
        {8,       1,         10},
        {2,       3,          5},
        {1,       1,          6},
        {6,       5,          4},
        {4,       3,          4},
        {5,       1,         15},
        {7,       6,         30},
        {1,       2,          6},
        {6,       2,          9},
};

 printf("UNSORTED LIST\n\n");
 for (i=0;i<50;i++){
 item[i]=50-i;
 printf(“Element %d is: %d\n”,i+1,item[i]);
 }

 selection_sort(item, 50);
 printf("\n\nSORTED LIST\n\n");
 for (i=0;i<50;i++)
 printf(“Element %d is: %d\n”,i+1,item[i]);

 return(0);
}

void selection_sort(int *item, int n_items)
{
     int i, j, min, minindex, temp;

     for(i = n_items-1; i < 0; i--)
     {
         min = item[i];
         minindex = i;
         for(j = n_items; j < i+1; j--)
         {
             if (item[j] < min)
             {
                 min = item[j];
                 minindex = j;
             }
         }
     temp = item[i];
     item[i] = min;
     item[minindex] = temp;
     }
}
