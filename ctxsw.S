/* Summary
ctxsw performs 4 basic steps

1. Push values of registers onto the current process stack
2. Save stack pointer in process table for current process, load stack pointer for new process
3. Reload processor registers from values previously saved on the new process' stack
4. Jump to location in the new process at which execution should resume

To save contents of processor registers
- ctxsw allocates CONTEXT bytes on the stack & stores register values
- ctxsw receives (address of location of current process stack pointer, address of new stack pointer)
- stack pointe rpoints to new process' stack
- Ctxsw extracts set of values that was saved on the stack for the process & loads the values into processor registers. 
- Once values have been loaded, ctxsw removes CONTEXT bytes from the stack

Newly created process should execute w/ interupts enabled. Other processes will return from the call to context switch & execute resched w/ interrupts disabled. Context switch must use co-processor to enable interrupts explicitly. Ctxsw compares 2 values & enables interrupts if they differ.

Instead of saving the value of the program counter while ctxsw is running, choose an address at which the process should resume when restarted

The process has called resched which has then called ctxsw. Code saves a value as if the process had just returned from the call to ctxsw. The saved value of hte program counter is taken from the return address, the address to which ctxsw would return if it were a normal procedure.

When a process restarts, the process begins executing in resched immediately following the call to ctxsw

Each process has its own stack of function calls - when a given process resumes execution & returns from resched, the return may go to a diff caller than the return in another process

In Xinu, each function including the scheduler & context switch, eventually returns to its caller
*/




