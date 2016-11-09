#include<stdio.h>
#include<time.h>
int ack(int m, int n)
{
    if (m == 0)
        return n + 1;
    else if (m > 0 && n == 0)
        return ack(m - 1, 1);
    else if (m > 0 && n > 0)
      return ack(m - 1, ack(m, n - 1));
  else
    return -1;
}

int main()
{
    int m = 2, n = 1, ans = 0;

    //scanf("%d",&m);
    //scanf("%d",&n);        

    ans = ack(m, n);
    return 0;
}
