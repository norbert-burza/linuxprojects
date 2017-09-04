#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

void semafor_zielone(int, int);
void semafor_czerwone(int, int);

int main(int argc, char *argv[])
{
    int semid;
    //Wczytanie semid
    if (argc != 2)
    {
        printf("konsument: należy podać semid\n");
        exit(1);
    }
    char * ptrEnd;
    semid = strtol(argv[1], &ptrEnd ,10);
    //***************

    //Otwarcie pliku roboczego "konsumpcja"
    FILE * konsumpcja, * towar;
    konsumpcja = fopen("konsumpcja", "w");
    if (konsumpcja == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //*************************************

    char znak;
    for ( ; ; )
    {
        semafor_czerwone(semid, 1); //TRWA KONSUMPCJA
        //Odczyt znaku z bufora
        towar = fopen("towar", "r");
        if (towar == NULL)
        {
            perror("fopen()");
            exit(1);
        }
        znak = fgetc(towar);
        if (fclose(towar) == EOF)
        {
            perror("fclose()");
            exit(1);
        }
        //*********************

        semafor_zielone(semid, 0); //UMOŻLIWIENIE PRODUKCJI

        //Sprawdzenie znaku
        if (znak == EOF) break;
        //*****************

        //Zapis znaku do konsumpcji
        fputc(znak, konsumpcja);
        //*************************
    }

    //Zamknięcie plików roboczych
    if (fclose(konsumpcja) == EOF)
    {
        perror("fclose()");
        exit(1);
    }
    //***************************
    return 0;
}

//Funkcje do obsługi semaforów
void semafor_zielone(int semId, int sem_num)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = sem_num; /* semaphore number */
	sembuf_struct.sem_op = 1; /* semaphore operation */
	sembuf_struct.sem_flg = 0; /* operation flags */

	int result;
	do
	{
	    result = semop(semId, &sembuf_struct, 1);
	}
	while (result == -1 && errno == EINTR);

	if (result == -1 && errno != EINTR)
	{
        perror("semop()");
        exit(3);
	}
}

void semafor_czerwone(int semId, int sem_num)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = sem_num; /* semaphore number */
	sembuf_struct.sem_op = -1; /* semaphore operation */
	sembuf_struct.sem_flg = 0; /* operation flags */

	int result;
	do
	{
	    result = semop(semId, &sembuf_struct, 1);
	}
	while (result == -1 && errno == EINTR);

	if (result == -1 && errno != EINTR)
	{
        perror("semop()");
        exit(3);
	}
}
