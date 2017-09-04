#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<unistd.h>


int main() {

        printf("UID: %d\t",getuid()); //identyfikator uzytkownika
        printf("GID: %d\t",getgid()); //identyfikator grupy
        printf("PID: %d\t",getpid()); //identyfikator procesu
	printf("PPID: %d\n",getppid()); //identyfikator procesu macierzystego
	printf("PGID:   %d\n",getpgid(0));//dopisany przez JW
	

	return 2;

}
