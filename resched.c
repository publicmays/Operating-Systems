/* Summary
Resched begins by checking global variable Defer.ndefers to see whether rescheduling is deferred. If so resched sets global var Defer.attempt to indicate that an attempt was made during the deferral period and returns to the caller. Deferred reschedulling is provided to accomodate device drivers for hardware that requires a driver to service multiple devices on a single interrupt.

Once it passes the test for deferral, resched examines an implicit parameter : the state of the current process. If the state var contains PR_CURR & the current process' priority is the highest in the system, resched returns & the current process remains running. If the state specifies that the current process should remain eligible to use the CPU but the current process doesn't have the highest priority, resched adds the current process to the ready list. Resched then removes the process at the head of the ready list & performs a context switch.
*/

#include <xinu.h>

// resched - reschedule processor to highest priority eligible process

void resched(void) {    // assumes interrupts are disabled
    struct procent *ptold; // ptr to table entry for old process
    struct procent *ptnew; // ptr to table entry for new process
    
    // if rescheduling is deferred, record attempt & return
    if(Defer.ndefers > 0) {
        Defer.attempt = TRUE;
        return;
    }
    
    // point to process table entry for the current (old) process
    ptold = &proctab[currpid]; // reference address
    
    if(ptold->prstate == PR_CURR { // process remains running
        if(ptold->prprio > firstkey(readylist)) {
            return;
        }
        
        // old process will no longer remain current
        ptold->prstate = PR_READY;
        insert(currpid, readylist, ptold->prprio);
        
        // force context switch to highest priority ready process
        currpid = dequeue(readylist);   // extract highest priority from readylist
        ptnew = &proctab[currpid];      // reference address
        ptnew->prstate = PR_CURR;       // new ptr is curren state
        preempt = QUANTUM;              // reset time slice for process
        ctxsw(&ptold->prstkptr, &ptnew->prstkptr);  // context switch old and new
        
        // old process returns here when resumed
        return;
    }
}