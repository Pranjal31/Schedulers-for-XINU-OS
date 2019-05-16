#ifndef AGESCHED
#define AGESCHED 1
#endif

#ifndef LINUXSCHED
#define LINUXSCHED 2
#endif
extern int schedclass;
void setschedclass(int);
int getschedclass();
extern int currprio;
extern int epoch;
