**Objective:** To implement two new scheduling policies that overcome the problem of process starvation:
	(1) Aging-based Scheduler
	(2) Linux-like Scheduler

**Motivation:** The default scheduler in Xinu will schedule the process with the higher priority. Starvation is observed in Xinu when there are two or more processes eligible for execution that have different priorities. The higher priority process gets to execute first which results in lower priority processes never getting any CPU time unless the higher priority process ends. 

**Introduction:**

*(1) Aging-based Scheduler:*
The first scheduling policy is an Aging Based Scheduler. The basic idea of this scheduler is to gradually increase the priority of the processes waiting to be executed every time there's a need for scheduling. In this scheduler, priorities of the processes waiting to be executed is incremented by 1 every time there is a need for scheduling (i.e every time resched() is called). In the case of processes with equal priorities round robin would be used which is the default existing Xinu Policy. 

For example, assume that there are three processes eligible for execution P1(1), P2(2), and P3(3) with priorities as mentioned. So if P3(3) is running and  P1(1) and P2(2) are waiting to be executed. When resched is called we will have new priorities of processes waiting to be executed as follows: P1(1) -> P1(2) and P2(2) -> P2(3).

```
Initially P3(3) is running and P1(1) and P2(2) are waiting to be executed. 
          
	  Ready Queue            
             after                                   
          incrementing     Currently Running      Process Scheduled      New Ready Queue 
	  
1st Call: P1(2) P2(3)      P3(3) Running          P2(3) Scheduled        P1(2) P3(3)

2nd Call: P1(3) P3(4)      P2(3) Running          P3(4) Scheduled        P1(3) P2(3)
```

*(2) Linux-like Scheduler:*
This scheduling algorithm tries to loosely emulate the Linux scheduler in 2.2 kernel. With this algorithm, the CPU time is divided into epochs. In each epoch, every process has a specified time quantum, whose duration is computed at the beginning of the epoch. An epoch will end when all the runnable processes have used up their quantum. If a process has used up its quantum, it will not be scheduled until the next epoch starts, but a process can be selected many times during the epoch if it has not used up its quantum.

When a new epoch starts, the scheduler will recalculate the time quantum of all processes (including blocked ones). This way, a blocked process will start in the epoch when it becomes runnable again. New processes created in the middle of an epoch will wait till the next epoch, however. For a process that has never executed or has exhausted its time quantum in the previous epoch, its new quantum value is set to its process priority (i.e., quantum = priority). A quantum of 10 allows a process to execute for 10 ticks (10 timer interrupts) within an epoch. For a process that did not get to use up its previously assigned quantum, part of the unused quantum is allowed to be carried over to the new epoch. Suppose for each process, a variable counter describes how many ticks are left from its quantum, then at the beginning of the next epoch, quantum = floor(counter/2) + priority. For example, a counter of 5 and a priority of 10 will produce a new quantum value of 12. During each epoch, runnable processes are scheduled according to their goodness. For processes that have used up their quantum, their goodness value is 0. For other runnable processes, their goodness value is set considering both their priority and the amount of quantum allocation left: goodness = counter + priority. Note that round-robin is used among processes with equal goodness.

Priority changes made in the middle of an epoch will only take effect in the next epoch. An example of how processes should be scheduled under this scheduler is as follows:
If there are processes P1, P2, P3 with time quantum 10,20,15 then the epoch would be equal to 10+20+15=45 and the possible schedule (with quantum duration specified in the braces) can be: P2(20), P3(15), P1(10), P2(20) ,P3(15), P1(10) but not: P2(20), P3(15), P2(20), P1(10).
