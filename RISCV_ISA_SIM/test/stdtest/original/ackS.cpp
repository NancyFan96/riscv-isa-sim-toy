#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 1000000
#define CHECK_TIME

int ackS(int m, int n){
    int sp = 0;
    int s1[MAX], s2[MAX];
    int ans;
    
    s1[sp]=m;
    s2[sp]=n;
    while(sp!=0 || s1[sp]!=0){
        while(s1[sp]!=0){
            while(s2[sp]!=0){
                // m,n > 0 | push
                sp++;
                s1[sp]=s1[sp-1];
                s2[sp]=s2[sp-1]-1;
            }
            //m!=0,n=0,exchange to (m-1,1)
            s1[sp]--;
            s2[sp]=1;
        }
        //m=0, return n+1, exchange to (m-1, ReturnValue) | pop
        sp--;
        s1[sp]--;
        s2[sp]=s2[sp+1]+1;
    }
    sp--;
    ans = s2[sp+1]+1;
    
    return ans;
}

int main()
{
    int ans;
    int m, n;
    
    scanf("%d%d",&m,&n);
    
#ifdef CHECK_TIME
    clock_t Start_Time, End_Time;
    Start_Time = clock();
    ans = ackS(m,n);
    End_Time = clock();
    printf("Duration(s):%lf\n", (double)(End_Time-Start_Time)/CLOCKS_PER_SEC);
#endif
#ifndef CHECK_TIME
    ans = ackS(m,n);
#endif
    
    printf("ack(%d,%d)=%d\n",m,n,ans);
    
    return 0;
}

