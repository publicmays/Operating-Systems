/*  Kathleen Lew
I initialize all of my forks to a value of 1
I initialize all of my philosophers to its index 
Then I created 5 processes with equal priority
The process goes around in a circle
A philosopher will acquire up forks 1 & 2 
- Acquire calls the function wait on that fork
- Wait will decrement the count of the semaphore 
- A philosopher can pick up a fork if it is free but if it is not...
- Wait will then enqueue the calling process(fork) on the list for the semaphore
- changes the process state to PR_WAIT
- and calls resched to switch to the ready process

- This way 5 philosopher processes are all running concurrently
- however some can be in a state of PR_WAIT

- If forks on both sides of the philosopher are open
 - the philosopher can then eat
- After the philosopher eats, the philosopher can release both forks
- Release calls signal
- Release takes the semaphore ID (fork) and increments the count of the semaphore
- It makes the 1st process ready if any are waiting
- This is how other forks can become available for other philosophers to pick up
- Then the philosopher thinks
- NOTE - There are multiple versions where philosophers think and eat at different periods of time

- These semaphores prevent deadlock because all philosophers get to eat
- Philosophers go around in a circular buffer 


The shared resources are ordered (0 to N-1 for the forks).
- A process that needs several resources must acquire them in
increasing order with respect to the global order on resources.
- No deadlock is then possible.

Hypothetical deadlock situation, consider the process
that holds the highest order resource. This process has already acquired all the lower order resources that
it might need and thus cannot be blocked.

Furthermore, assuming the the semaphores are fair, not process can be
blocked forever.

Indeed, if a process is waiting for a fork, semaphore fairness ensures
that it will has access to it as soon as it is freed.

For a fork never to be freed, the process holding it must be waiting for
a higher-order fork.

Since the chain of blocking waits takes us up in the fork order, it must
stop when the highest-order fork is reached. Indeed, this one will
necessarily be the last one acquired by the process using it and will
thus eventually be freed.

Mutexs can be used but they're not required
*/

#include <xinu.h>
#include <stdio.h>
#define FORKS 5
#define PHILOSOPHERS 5
int i;
sid32 forks[FORKS];
int philosophers[PHILOSOPHERS];

void philosopherDinner(int i);
void acquire(sid32 fork);
void eat(int i);
void think(int i);
void initialize();

int main(int argc, char **argv) {

	for(i=0; i < FORKS; ++i){
		forks[i] = semcreate(1);
		philosophers[i] = i;
	}


	for(i = PHILOSOPHERS-1; i >= 0; i--) {
		resume( create(philosopherDinner, 1024, 20, "philosopher", 1, i) );
	
	}
}
void philosopherDinner(int i) {
	while(TRUE){	
		int i1 = i;
		int i2 = (i+1)%FORKS;


		//kprintf("1 - %d\n",i);
		acquire(forks[i2]);
		acquire(forks[i1]);
		
		//kprintf("2 - %d\n",i);
		eat(i);
		//kprintf("3 - %d\n",i);
		release(forks[i2]);
		release(forks[i1]);
		//kprintf("4 - %d\n",i);
		think(i);
		//kprintf("5 - %d\n",i);
	}
	
}

void acquire(sid32 fork) {
	wait(fork); // pick them up, are only mutual exclusions
}
void release(sid32 fork){
	signal(fork); // put them down
}
void eat(int i) {
	kprintf("Philosopher %d is eating\n\n", (i+1));
}
void think(int i) {
	kprintf("Philosopher %d is thinking\n\n" , (i+1));
	sleep(2);
}
