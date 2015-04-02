
/* who_wc.c  */
/* demonstrates a one-way pipe between two processes.
This program implements the equivalent of the shell command:

who | wc -l

which will count the number of users logged in.
*/

#include <stdio.h>

/* Define some manifest constants to make the code more understandable */

#define ERR    (-1)             /* indicates an error condition */
#define READ   0                /* read end of a pipe */
#define WRITE  1                /* write end of a pipe */
#define STDIN  0                /* file descriptor of standard in */
#define STDOUT 1                /* file descriptor of standard out */

int main()
{
	int   pid_1,               /* will be process id of first child - who */
	      pid_2,               /* will be process id of second child - wc */
	      pfd[2];              /* pipe file descriptor table.             */

	if ( pipe ( pfd ) == ERR )              /* create a pipe  */
	      {                                 /* must do before a fork */
	      perror (" ");
	      exit (ERR);
	      }
	if (( pid_1 = fork () ) == ERR)        /* create 1st child   */
	      {
	      perror (" ");
	      exit (ERR);
	      }
	if ( pid_1 != 0 )                      /* in parent  */
	      {
	      if (( pid_2 = fork () ) == ERR)     /* create 2nd child  */
	            {
	            perror (" ");
	            exit (ERR);
	            }
	      if ( pid_2 != 0 )                   /* still in parent  */
	            {
	            close ( pfd [READ] );         /* close pipe in parent */
	            close ( pfd [WRITE] );        /* conserve file descriptors */
	            wait (( int * ) 0);           /* wait for children to die */
	            wait (( int * ) 0);
	            }

	      else                                /* in 2nd child   */
	            {
	            close (STDIN);           /* close standard input */
	            dup ( pfd [READ] );      /* read end of pipe becomes stdin */
	            close ( pfd [READ] );            /* close unneeded I/O  */
	            close ( pfd [WRITE] );           /* close unneeded I/O   */
	            execl ("/usr/bin/wc", "wc", "-l", (char *) NULL);
	            }	
	      }
	else                                      /* in 1st child   */
	      {
	      close (STDOUT);            /* close standard out	 */
	      dup ( pfd [WRITE] );       /* write end of pipes becomes stdout */
	      close ( pfd [READ] );                 /* close unneeded I/O */
	      close ( pfd [WRITE] );                /* close unneeded I/O */
	      execl ("/usr/bin/who", "who", (char *) NULL);
	      }
	exit (0);
}
