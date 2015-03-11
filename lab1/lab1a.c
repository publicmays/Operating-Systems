/*  Kathleen Lew 
a. Analyze what can go wrong
- Producer is more productive and can't guarantee order of exclusion
- Producer may try to produce into a full buffer
- Opposite can be said about the consumer
- If the consumer consumes things too fast, he cannot consume from an empty buffer
- Race condition can lead to 
- Deadlock can also happen where both processes are waiting to be awakened
- This is why sempahores can be used for mutual exclusion & synchronization

The counting semaphore psem keeps track of empty slots
and the counting semaphore csem keeps track of full slots.

Array is implemented like a queue
- Critical section - always will produce and consume 

- Consumer notifies producer when final element in queue is dequeued
- If queue is filled up, the producer must notify the consumer

- Mutex semaphore will make sure critical section is not interferred with 
and the producer cannot interfere while the consumer is accessing the critical section
vice versa. This prevents interference

*/

#include <xinu.h>
#include <stdio.h>
#define P_C_Q_SIZE 50

sid32 psem;					// 50 available
sid32 csem;					// 0 available, 50 are taken
sid32 mutex;
int32 n = 0;
int32 head = 0; 				//queue index
int32 tail = 0;
int32 count = 0;
int32 myQueue[P_C_Q_SIZE];

void produce(sid32 psem, sid32 csem, sid32 mutex);
void consume(sid32 psem, sid32 csem, sid32 mutex);
void putItem(int32 n);
int32 extractItem();

int main(int argc, char **argv) {

	psem = semcreate(P_C_Q_SIZE);			// producer semamphore 50 available slots
	csem = semcreate(0);					// consumer semaphore 0 available slots
	mutex = semcreate(1);					
	resume( create(consume, 1024, 20, "cons", 3, psem, csem, mutex));		// consumer process
	resume( create(produce, 1024, 20, "prod", 3, psem, csem, mutex));		// producer process
	
	return OK;
}

void produce(sid32 psem, sid32 csem, sid32 mutex) {
	while(count < P_C_Q_SIZE) {
		wait(psem);				// interrupts are disabled, decrements
								// if it's les sthan 0, sets it to waiting
								//  puts semaphore inside the process
								// puts it into the sempahores
		wait(mutex);
		kprintf("Produced %d\n", n);
		putItem(n++);			// declare n outside
		signal(mutex);
		signal (csem);
		count++;

	}

}

void consume(sid32 psem, sid32 csem, sid32 mutex) {
	int32 counter;
	while(counter < P_C_Q_SIZE){
		wait(csem);
		wait(mutex);
		int extract = extractItem(); 			// extract top item

		kprintf("Consumed %d\n", extract);
		signal(mutex);
		signal(psem);
		counter++;
	}
}

void putItem(int32 n){
	int32 mod = (P_C_Q_SIZE-tail-1) % P_C_Q_SIZE;
	myQueue[mod] = n;
	tail++;
}

int32 extractItem() {
	int pos = head % P_C_Q_SIZE;	
	
	head++;
	return myQueue[pos];
	
	
}