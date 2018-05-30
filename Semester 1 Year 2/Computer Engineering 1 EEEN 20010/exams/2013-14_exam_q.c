int myfunc(int , int );

int main(void)
{

 int x = 2, y = 5;
 printf("Before: %d %d\n", x, y);
 myfunc(x,y);

 return(0);
}

int myfunc(int a, int b)
{
int temp;

 temp = a;
 a = b;
 b = temp;
 printf("After: %d %d\n", a, b);

}
