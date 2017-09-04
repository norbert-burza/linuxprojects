#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<error.h>


int main() {
	 char tree[10];
	int i,mat_id,w,x;
 	mat_id=getpid();
        sprintf(tree,"pstree -ph %d",mat_id);
	

	

	printf("Proces macierzysty\nUID: %d\tGID: %d\tPID: %d\tPPID: %d\n\n",getuid(),getgid(),getpid(),getppid());

 	for(i=1; i<=3; i++) {
        	switch(fork()) {
         		case -1:
                		perror("Nie utworzono procesu potomnego"); 
         		exit(1);

         		case 0:    execl("./program1_1","program1_1", NULL);
                		//printf("Fork %d - UID: %d\tGID: %d\tPID: %d\tPPID: %d\n",i,getuid(),getgid(),getpid(),getppid());	
                	break;

         		default:
 				printf("macierzysty %d\n", i);}
        	
	         
        	sleep(1);
}
  		
		
	
        system(tree);
         for(i=0;i<3;i++){
                fprintf(stdout,"Teaz czekam na zakonczenie potomka\n");
	        w = wait(&x);
	        fprintf(stdout, "Skonczyl sie potomny PID=%d\n ze znakiem %d\n",w,x);}
	return 0;

}


