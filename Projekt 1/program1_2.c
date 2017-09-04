#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<error.h>


int main() {
        char tree[10];
	int i,mat_id;
 	mat_id=getpid();
        sprintf(tree,"pstree -ph %d ",mat_id);
	printf("Proces macierzysty\nUID: %d\tGID: %d\tPID: %d\tPPID: %d\n\n",getuid(),getgid(),getpid(),getppid());

 	for(i=1; i<=3; i++) {
        	switch(fork()) {
         		case -1:
                		perror("Nie utworzono procesu potomnego");
         		exit(1); break;

         		case 0:
                		printf("Fork %d - UID: %d\tGID: %d\tPID: %d\tPPID: %d\n",i,getuid(),getgid(),getpid(),getppid());	
                	break;

         		default:
 				printf("macierzysty %d\n", i);
				fork();//dopisany przez JW
                                }

  		sleep(2);
		
	}

        system(tree);
	return 0;

}

/*

|-8506
  | | |
  | | |-8507
  | |   |  |-8509
  | |   |      |-8509
  | |   |
  | |   |-8511
  | |
  | |-8508
  |     |-8512
  |
  |-8510

*/
