#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include  <pthread.h> 

int howLongToSleep = 1;

void semafor_zielone(int);
void samafor_czerwone(int);
int ilosc_procesy_czekajace(int);
int stan_semafora(int);

int liczba_sk, semid;
char *ptrEnd;
int old_cancel_state;

int main(int argc, char *argv[]) /*liczba_sk, sem_id*/
{
    if (argc != 3)
    {
        printf("proces_potomny: błędna ilość argumentów\n");
        exit(1);
    }
    else
    {
        liczba_sk = strtol(argv[1], &ptrEnd, 10);
        semid = strtol(argv[2], &ptrEnd, 10);
    }

    int i;
    for (i=0; i<liczba_sk; i++)
    {
pthread_setcancelstate  (PTHREAD_CANCEL_DISABLE, &old_cancel_state);
    samafor_czerwone(semid);
/*Sekcja krytyczna*/  printf("proces_potomny\tPID: %d - w sekcji krytycznej, usypiam na %ds\n", (int)getpid(), howLongToSleep);
/*----------------*/  printf("Stan semafora semval=%d\n", stan_semafora(semid));
/*----------------*/  sleep(howLongToSleep);
/*----------------*/  printf("Ilość procesów oczekujących na dostęp do sekcji krytycznej: %d\n", ilosc_procesy_czekajace(semid));
/*----------------*/
/*----------------*/
/*Sekcja krytyczna*/
    semafor_zielone(semid);
pthread_setcancelstate (old_cancel_state,  NULL);
    printf("proces_potomny\tPID: %d Stan semafora po wyjściu z sekcji krytycznej: semval=%d\n", (int)getpid(), stan_semafora(semid));
    }
}

void samafor_czerwone(int semid)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = 0; /* semaphore number */
	sembuf_struct.sem_op = -1; /* semaphore operation */
	sembuf_struct.sem_flg = SEM_UNDO; /* operation flags */

    if (semop(semid, &sembuf_struct, 1) == -1)
	{
		perror("semop");
		exit(3);
	}
}

void semafor_zielone(int semid)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = 0; /* semaphore number */
	sembuf_struct.sem_op = 1; /* semaphore operation */
	sembuf_struct.sem_flg = SEM_UNDO; /* operation flags */

    if (semop(semid, &sembuf_struct, 1) == -1)
	{
		perror("semop");
		exit(3);
	}
}

int stan_semafora(int semid)
{
    int stan;
    if ((stan=semctl(semid, 0, GETVAL)) == -1)
    {
        perror("semctl");
        exit(3);
    }
    else
    {
        return stan;
    }
}

int ilosc_procesy_czekajace(int semid)
{
    int ilosc;
    if ((ilosc=semctl(semid,0,GETNCNT)) == -10)
    {
        perror("semctl");
        exit(3);
    }
    else
    {
        return ilosc;
    }
}
