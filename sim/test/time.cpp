#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>

int main()
{
	struct tms      time_info;
	long            Begin_Time;
	
	times (&time_info);
	Begin_Time = (long) time_info.tms_utime;
	printf ("Begin_Time(long%lx) = %lf\n", Begin_Time, Begin_Time);

	return 0;
}
