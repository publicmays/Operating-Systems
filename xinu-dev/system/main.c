/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

void myprocess(int a){
	kprintf("Hello World %d\n", a);
}

void myhungryprocess(void) {
	kprintf("I'm a looping process\n");
	while (TRUE) {
		// do nothing forever
	}
}
int main(int argc, char **argv)
{
	uint32 retval;

	// resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	// my code
	resume(create(myhungryprocess, 4096, 1000, "hungryprocess", 0));
	resume(create(myprocess, 4096, 50, "helloworld1", 1, 1));
	resume(create(myprocess, 4096, 50, "helloworld2", 1, 2));
	resume(create(myprocess, 4096, 50, "helloworld3", 1, 3));
	resume(create(myprocess, 4096, 50, "helloworld4", 1, 4));
	// -- end of my code
    while (TRUE) {
		// Do nothing
	}

	/* Wait for shell to exit and recreate it */

	recvclr();
	while (TRUE) {
		retval = receive();
		kprintf("\n\n\rMain process recreating shell\n\n\r");
		resume(create(shell, 4096, 1, "shell", 1, CONSOLE));
	}
	while (1);

	return OK;
}
