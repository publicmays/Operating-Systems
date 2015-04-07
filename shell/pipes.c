#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


void strrev(char *p)
{
  char *q = p;
  while(q && *q) ++q;
  for(--q; p < q; ++p, --q)
    *p = *p ^ *q,
    *q = *p ^ *q,
    *p = *p ^ *q;
}

int main (int argc, char **argv)
{
	pid_t pid[3]; 				//list of processes
	int ab[2], bc[2], ca[2]; 	//create as many pipes as you need;
	//init pipes

	pipe(ab);
	pipe(bc);
	pipe(ca);

	//fork child
	pid[0]=fork(); //Fork first process
	if(pid[0] == 0) //
	{
			

	        
	        
			
			char buffer[100];

		    fgets(buffer, sizeof(buffer), stdin);
		    int len = strlen(buffer);

		    if (write(ab[1], buffer, len) != len)
		        printf("Failed to write to children");
		    if (read(ca[0], buffer, len) != len)
		        printf("Failed to read from children");
		    
		    printf("%.*s", len, buffer);
	        
	        for(int i=0; i<2; i++)
	        {close(ab[i]); close(bc[i]); close(ca[i]); }

	        //dup2(STDOUT_FILENO, ab[1]);
	        //printf("%s", str2);
	        // quit so your child doesn't end up in the main program
	        exit(0);
	}

	pid[1]=fork();
	if(pid[1] == 0)
	{
	        
	        // Process B reads from process A
	        dup2(ab[0], STDIN_FILENO);
	        // Process B writes to process C
	        dup2(bc[1], STDOUT_FILENO);
	        
	        for(int i=0; i<2; i++)
	        { close(ab[i]); close(bc[i]); close(ca[i]); }

	        char str[100];
	        scanf ("%[^\n]%*c", str);
	        strrev(str);
	        printf("%s\n", str);

	        exit(0);
	}

	pid[2]=fork();
	if(pid[2] == 0)
	{
	        
	        // Process C reads from process B
	        dup2(bc[0], STDIN_FILENO);
	        // Process C writes to process A
	        dup2(ca[1], STDOUT_FILENO);
	        for(int i=0; i<2; i++)
	        { close(ab[i]); close(bc[i]); close(ca[i]); }
	    	char str[100];        
	        scanf ("%[^\n]%*c", str);
	        int i = 0;
			while (str[i]){
			    
			    str[i] = toupper(str[i]);
			    i++;
			}

	        
	        printf("%s\n", str);
	        
	        exit(0);
	}

	
	for(int i=0; i<2; i++)
	{ close(ab[i]); close(bc[i]); close(ca[i]); }

	
    wait();
    wait();
    wait();
    
	
	//How can you implement this logic for n processes?
	//This will help you implement piped commands on your shell
}