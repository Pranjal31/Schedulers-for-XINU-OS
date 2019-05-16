#include <lab1.h>

int schedclass;
void setschedclass(int sched_class){
if(sched_class != AGESCHED && sched_class != LINUXSCHED)
	return;
schedclass = sched_class;
}

int getschedclass(){
return schedclass;
}

