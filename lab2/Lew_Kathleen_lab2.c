/* Kathleen Lew 1218 8386 Lab 2 main.c */
# include <xinu.h>
# include <stdio.h>
# include <stdint.h>

/* Constants */
# define QUEUE_SIZE 10
# define MAX_MESSAGES 10
# define MAX_RECIPIENT_PROCESSES 3

/***********************************************************
 * GLOBAL VARIABLES
 ***********************************************************/

/*--------------------------------------------------------------------
 * STRUCT messageQueue - will hold messages and have a pointer to tail & head
 * Holds the message count and an array of messages max size of 10 
 *--------------------------------------------------------------------*/
struct messageQueue {
        int32 head, tail, messageCount;
        umsg32 messages[MAX_MESSAGES];
};

/* Array of messageQueues that holds messageQueue */
struct messageQueue messageQueues[NPROC];
struct messageQueue* processMsgQueue;
int32 difference, isSend = FALSE, i = 0;
umsg32 temp;
uint32 successfulMessages = 0, successfulPIDs = 0;

/* This is from send.c */
intmask mask;                       /* saved interrupt mask     */
struct procent *prptr;              /* ptr to process' table entry */
/*Initialize queue for receiver*/
void initializeQueue(struct messageQueue* queue) {
    queue->head = queue->tail = queue->messageCount = 0;
}
/*------------------------------------------------------------------------
 * finish - restores mask and returns SYSERR or OK
 ------------------------------------------------------------------------*/
int32 finishError(intmask mask) {
    restore(mask);
    return SYSERR;
}
int32 finish(intmask mask) {
    restore(mask);
    return OK;
}
/*------------------------------------------------------------------------
 * general Checks disable mask and check pid, sets process pointer to pid
 ------------------------------------------------------------------------*/
void generalChecks(intmask mask, pid32 pid, int32 isSend){
    mask = disable();
    if(isSend == TRUE){ 
        if (isbadpid(pid)) {
            printf("GENERAL CHECK - bad pid\n");
            finishError(mask);
        }
    }
    prptr = &proctab[pid];
}
/*------------------------------------------------------------------------
 * Queue functions start here
 ------------------------------------------------------------------------*/
/* Check - Is message queue empty */
uint32 empty(struct messageQueue* q) {
    return (q->messageCount == 0);
}
/* Check - is message queue full */
uint32 full(struct messageQueue* q) {
    return(q->messageCount == QUEUE_SIZE);

}
/* Enqueue */
uint32 enqueueMsg(struct messageQueue* q, umsg32 message) {
    if(full(q))
        return FALSE;
    else {
        q->messageCount += 1;
        q->tail += 1;
        q->messages[q->tail % QUEUE_SIZE] = message;
        return TRUE;
    }
}
/* Dequeue */
umsg32 dequeueMsg(struct messageQueue* q) {
    if(empty(q))
        return '0';
    else {
        q->messageCount -= 1;
        q->head += 1;
        return (q->messages[q->head % QUEUE_SIZE]);
    }
}

/*------------------------------------------------------------------------
 * sendMsg
 * ------------------------------------------------------------------------*/
syscall sendMsg(pid32 pid, umsg32 msg) {
    /* From send.c */
    generalChecks(mask, pid, TRUE);
    /* Check - This is not a running process */
    if(prptr->prstate == PR_FREE) {
        finishError(mask);
    }
    /*--------------------------- 
     * End of send.c checks
     * Start of sendMsg 
     *--------------------------*/
    // create a process message queue 
    processMsgQueue = &messageQueues[pid];
    // Difference - how many messages in myQueue w/ current pid
    difference = processMsgQueue->tail - processMsgQueue->head;

    /* Check - process does not have a message 
     * Call send, put message in procent */
    if((prptr->prhasmsg) == 0) {
        kprintf("Message %c ---- to Receiver: %d.\n", msg, pid);
        if(send(pid, msg) != OK) {
            finishError(mask);
        }
        /* Call Ready */
        if(processMsgQueue->messageCount < difference){
            ready(pid, RESCHED_YES);
        }
        finish(mask);
    }

    /*---------------------------
     * There's already a message in procent
     * Put it in the queue 
     *------------------------------*/
    else {
        /* Check - if msg queue for process is full, throw ERROR */
        if(full(processMsgQueue)) {
            printf("Queue is full.\n");
            finishError(mask);
        }
        /*--------------------
         * ENQUEUE
         *--------------------*/
        if(enqueueMsg(processMsgQueue, msg) == TRUE) {
            if(difference > processMsgQueue->messageCount)
                ready(pid, RESCHED_YES);

            kprintf("Enqueue Message %c ---- to Receiver: %d.\n", msg, pid);
            finish(mask);
        }
        else {
            finishError(mask);
        }
    }
}
/*------------------------------------------------------------------------
 * sendnMsg
 ------------------------------------------------------------------------*/
uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg) {
    uint32 sucessfulPIDs = 0;
    uint32 isOK = FALSE;
    uint32 i;
    for(i = 0; i < pid_count; ++i){

        /*------------Start of sendMsg--------------*/
        /* From send.c */
    pid32 pid = pids[i];
    int mask = disable();
    struct procent* prptr;
    prptr = &proctab[pid];
    struct messageQueue* processMsgQueue;
    /*--------------------------- 
     * End of send.c checks
     * Start of sendMsg 
     *--------------------------*/
    // create a process message queue 
    processMsgQueue = &messageQueues[pid];
    // Difference - how many messages in myQueue w/ current pid
    difference = processMsgQueue->tail - processMsgQueue->head;

    /* Check - process does not have a message 
     * Call send, put message in procent */
    if((prptr->prhasmsg) == 0) {
        kprintf("Message %c ---- to Receiver: %d.\n", msg, pid);

    }

    /*---------------------------
     * There's already a message in procent
     * Put it in the queue 
     *------------------------------*/
    else {

        if(enqueueMsg(processMsgQueue, msg) == TRUE) {
        /*--------------------
         * Don't call ready yet but set isOK = TRUE
         * because you sent the message successfully
         *--------------------*/
           /* if(difference > processMsgQueue->messageCount)
                ready(pid, RESCHED_YES);*/
            isOK = TRUE;
            kprintf("Enqueue Message %c ---- to Receiver: %d.\n", msg, pid);
            //finish(mask);
        }

    }
        /*------------End of sendMsg-----------------*/
        if(isOK == TRUE){
            ++sucessfulPIDs;
            /* Reset isOK back to false so you can run sendMsg again */
            isOK = FALSE;
        }
    }
    finish(mask);
    return sucessfulPIDs;
}

/*------------------------------------------------------------------------
 * sendMsgs
 ------------------------------------------------------------------------*/
uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count) {
    uint32 sucessfulMessages = 0;
    uint32 isOK = FALSE;
    uint32 i;
    for(i=0; i < msg_count; ++i) {
    /* Now within for loop do everything as sendMsg except 
     * instead of msg it is msgs[i] 
     */
        /*------------Start of sendMsg--------------*/
        /* From send.c */
    int mask = disable();
    struct procent* prptr;
    prptr = &proctab[pid];
    struct messageQueue* processMsgQueue;
    /*--------------------------- 
     * End of send.c checks
     * Start of sendMsg 
     *--------------------------*/
    // create a process message queue 
    processMsgQueue = &messageQueues[pid];
    // Difference - how many messages in myQueue w/ current pid
    difference = processMsgQueue->tail - processMsgQueue->head;

    /* Check - process does not have a message 
     * Call send, put message in procent */
    if((prptr->prhasmsg) == 0) {
        kprintf("Message %c ---- to Receiver: %d.\n", msgs[i], pid);
        
        if(send(pid, msgs[i]) != OK) {
            finishError(mask);
        }
        finish(mask);
    }

    /*---------------------------
     * There's already a message in procent
     * Put it in the queue 
     *------------------------------*/
    else {

        if(enqueueMsg(processMsgQueue, msgs[i]) == TRUE) {
        /*--------------------
         * Don't call ready yet but set isOK = TRUE
         * because you sent the message successfully
         *--------------------*/
           /* if(difference > processMsgQueue->messageCount)
                ready(pid, RESCHED_YES);*/
            isOK = TRUE;
            kprintf("Enqueue Message %c ---- to Receiver: %d.\n", msgs[i], pid);
            finish(mask);
        }

    }
        /*------------End of sendMsg-----------------*/
        /* If sendMsg ends up returns okay */
        if(isOK == TRUE){
            ++sucessfulMessages;
            /* Reset isOK back to false so you can run sendMsg again */
            isOK = FALSE;
        }
    }
    return sucessfulMessages;
     /* Call Ready */
  //  ready(pid, RESCHED_YES);
}


/*------------------------------------------------------------------------
 * receiveMsg - complete
 ------------------------------------------------------------------------*/
umsg32 receiveMsg(void) {
    intmask mask = disable();
    struct procent* prptr;
    prptr = &proctab[getpid()];
    struct messageQueue* currentMessageQueue;
    
    currentMessageQueue = &messageQueues[getpid()];

    /* Check if queue is empty, no message waiting on queue */
    if(empty(currentMessageQueue)) {
            printf("Queue is empty.\n");
            restore(mask);
            return receive();
    }
 
    /*-----------------------------
     * There are messages in queue, 
     * save it 
     * -----------------------------*/
    temp = prptr->prmsg;
    prptr->prmsg = dequeueMsg(currentMessageQueue);

    restore(mask);
    return temp;
}

/*------------------------------------------------------------------------
 * receiveMsgs
 ------------------------------------------------------------------------*/
syscall receiveMsgs(umsg32* messages, uint32 msg_count){

    /* Remember that instead of currpid you call getpid() */
    intmask mask = disable();
    struct procent* prptr;
    // address to process pointer of currpid
    prptr = &proctab[getpid()];

    struct messageQueue* currentMessageQueue;
    currentMessageQueue = &messageQueues[getpid()];

    /* Amount of messages in the current message queue tail - head */
    int32 difference = currentMessageQueue->tail - currentMessageQueue->head;

    /* Still need to wait on x amount of messages before you can start receiving */
    if(difference < msg_count) {
            prptr->prstate = PR_RECV;
            /* You don't have enough messages to receive it, you're still in receiving mode */
            resched();
    }

    for(i = 0; i < msg_count; ++i) {
        /* Start of receive */
            /* Check if queue is empty, no message waiting on queue */
            if(empty(currentMessageQueue)) {
                    printf("Queue is empty.\n");
                    restore(mask);
                    return receive();
            }
         
            /*-----------------------------
             * There are messages in queue, 
             * save it 
             * -----------------------------*/
            temp = prptr->prmsg;
            prptr->prmsg = dequeueMsg(currentMessageQueue);

            //restore(mask);
        /* End of receive */
        messages[i] = temp;
        printf("Dequeued Message %c ---- received from Receiver %d \n", messages[i], getpid());
        
    }
    finish(mask);
}


/*------------------------------------------------------------------------
 * PROCESSES - RECEIVER
 ------------------------------------------------------------------------*/
void receiver(void)
{
    kprintf("=== Start Receiver\n");
    umsg32 message = receiveMsg();
    intmask mask = disable();

    //Receive one message
    printf("Received: %c.\n", message);
    restore(mask);

    //Receive multiple messages
    sleep(5);
    int32 MESSAGE_COUNT = 5;
    umsg32 messages[MESSAGE_COUNT];

    receiveMsgs(messages, MESSAGE_COUNT);

    mask = disable();
    kprintf("=== End Receiver\n");
    restore(mask);

}

/*------------------------------------------------------------------------
 * PROCESSES - ANOTHER RECEIVER
 ------------------------------------------------------------------------*/
void receiver2(void) {
    umsg32 msgs[1];
    receiveMsgs(msgs, 1);
}
/*------------------------------------------------------------------------
 * PROCESSES - SENDER
 ------------------------------------------------------------------------*/
 void sender(pid32 pid, pid32 pid2){
    sleep(1);
    printf("Start Sender\n");

   
    umsg32 messages[10] = {'a','b','c','d','e','f','g','h','i','j'};
    /* Call sendMsgs */
    kprintf("Sent: %d messages --- to Receiver %d.\n", sendMsgs(pid, messages, 10), pid);

    int32 pids[2] = { pid, pid2 };

    /* Call sendnMsgs */
    kprintf("Sent message: %c --- to %d Receiver.\n", 'r', sendnMsg(2, pids, 'r'));

    intmask mask = disable();
    printf("Sender finished.\n");
    restore(mask);

}

int main(int argc, char **argv)
{
    for(i = 0; i < NPROC; ++i){
        initializeQueue(&messageQueues[i]);
    }

    /* Receivers will be created in suspend state */
    pid32 pid1 = create(receiver,8192,20,"r1",0);
    pid32 pid2 = create(receiver2,8192,20,"r2",0);

    resume(create(sender,8192,20,"sender",2,pid1, pid2));
    resume(pid1);
    resume(pid2);

    /* Make sure the main is no going to be the current process */
    chprio(getpid(),2);

    return OK;
}