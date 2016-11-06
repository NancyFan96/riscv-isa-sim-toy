#include<stdio.h>
#include<time.h>
#define CHECK_TIME
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
    int m, n, ans;

    scanf("%d",&m);
    scanf("%d",&n);        

#ifdef CHECK_TIME
    clock_t Begin_Time, End_Time;
    Begin_Time = clock();
    ans = ack(m,n);
    End_Time = clock();
    printf("Duration(s) :%lf\n",(double)(End_Time - Begin_Time)/CLOCKS_PER_SEC);
#endif
#ifndef CHECK_TIME
    ans = ack(m, n);
#endif

    printf("ans = %d\n", ans);

    return 0;
}
