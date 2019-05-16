/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int getmaxgoodness();
int getmaxgoodnessnode();
int currprio = 0;
int epoch = 0;

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int getmaxgoodness(){

int curr = q[rdyhead].qnext;
int maxgoodness = -1;
 while(curr != rdytail){
	if(q[curr].qkey > maxgoodness)
		maxgoodness = q[curr].qkey;
	
	curr = q[curr].qnext;
    }

return maxgoodness;
}


int getmaxgoodnessnode(){

int curr = q[rdyhead].qnext;
int maxgoodness = -1;
int maxgoodnessindex = 0;
 while(curr != rdytail){
        if(q[curr].qkey > maxgoodness){
                maxgoodness = q[curr].qkey;
		maxgoodnessindex = curr;
	}
        curr = q[curr].qnext;
    }

return maxgoodnessindex;

}

int resched()
{

/* Aging Scheduler - increase the priority of the processes in the ready queue*/

        if(getschedclass() == AGESCHED){
		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */
	
		int curr = q[rdyhead].qnext;
		while(curr != rdytail){
			q[curr].qkey += 1;
			curr = q[curr].qnext;
		}

		/* no switch needed if current process priority higher than next*/

		optr= &proctab[currpid];
		currprio = (currprio > optr->pprio)?currprio:optr->pprio;

		if (  (optr->pstate == PRCURR) &&
		   (lastkey(rdytail)<currprio)) { 
			return(OK);
		}
	
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,currprio);
		}

		/* remove highest priority process at end of ready list */
		currprio = lastkey(rdytail);	
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
	
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */
		return OK;
	}
	else if(getschedclass() == LINUXSCHED){
		int i;
		
		//beginning of new epoch
		if(epoch <= 0 || isempty(rdyhead) || (getmaxgoodness() <=0) ){   
			epoch=0;
			
			proctab[0].goodness = 0; // null process
			for(i=1; i<NPROC; i++){  // skip null process
				if(proctab[i].pstate != PRFREE){
	
					proctab[i].quant = proctab[i].pprio + (0.5 * proctab[i].quant_un);	//allot quanta for each proces
					proctab[i].quant_un = proctab[i].quant;

					proctab[i].goodness = proctab[i].pprio + proctab[i].quant_un;
					epoch += proctab[i].quant; 	//re-initialize epoch as sum of quanta
			
				}
		}

}

		  int curr = q[rdyhead].qnext;
          
                  while(curr != rdytail){
                  	q[curr].qkey = proctab[curr].goodness;
                        curr = q[curr].qnext;                                                                                                                                                                     }

		register struct pentry  *optr;  /* pointer to old process entry */
                register struct pentry  *nptr;  /* pointer to new process entry */
		optr= &proctab[currpid];

		if(currpid != 0){
			
			// old process will either go to WAIT state or READY state
               		epoch -= (optr->quant_un - preempt);
	                optr->goodness = optr->goodness - (optr->quant_un - preempt);
			optr->quant_un = preempt;
			
  	      	        if(optr->quant_un <= 0){
				optr->quant_un = 0;
                	 	optr->goodness = 0;
			}
			
		}

                /* no switch needed if current process goodness higher than next*/

                if (( optr->pstate == PRCURR) && !isempty(rdyhead) && (getmaxgoodness() < optr->goodness))
			return(OK);
			
		/* force context switch - old process going to READY state */

                if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid,rdyhead,optr->goodness);
                }
 	
		if(getmaxgoodness() > 0 && getmaxgoodnessnode() > 0 ){
			nptr = &proctab[ (currpid = dequeue(getmaxgoodnessnode())) ];   /* remove highest goodness process from the ready list */

			#ifdef  RTCLOCK
                        	preempt = nptr->quant_un;              /* reset preemption counter     */
                	#endif

		}
		else{
			nptr = &proctab[(currpid = dequeue(0))];             /* Schedule null process */
			#ifdef  RTCLOCK
                        	preempt = QUANTUM;              /* reset preemption counter     */
               		#endif

		}
	
                nptr->pstate = PRCURR;          /* mark it currently running    */
	        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
                /* The OLD process returns here when resumed. */
                return OK;	


	}

	else{


		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */

		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&	(lastkey(rdytail) <optr->pprio)) {
			return(OK);
		}
	
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif
		
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */
		return OK;


	}

}
