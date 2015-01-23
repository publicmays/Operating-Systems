/* queue.h - firstid, firstkey, isempty, lastkey, nonempty */
/* Queue structure declarations, constants & inline functions */
/* Default # of queue entries : 1 per process + 2 for ready list + 2 for sleep list + 2 per sempahore */

#ifndef NQENT
#define NQENT (NPROC + 4 + NSEM +  NSEM)
#endif

#define EMPTY (-1)          /* null value for qnext or qprev index */
#define MAXKEY 0X7FFFFFFF   /* max key that can be stored in queue */
#define MINKEY 0x80000000   /* min key that can be stored in queue */

struct qentry {             /* 1 per process + 2 per list */
  int32 qkey;               /* key on which queue is ordered */
  qid16 qnext;              /* index of next process or tail */
  qid16 qprev;              /* index of prev process or head */
};
