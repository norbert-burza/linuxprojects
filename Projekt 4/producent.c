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
int utworz_zestaw_semaforow();

int main()
{
    int semid; //semaphore set identifier
    //Utworzenie potrzebnych semaforów
    semid = utworz_zestaw_semaforow();
    printf("semid=%d\n", semid);
    //********************************

    FILE *surowce, *produkcja, *towar;

    //Otwarcie plików roboczych "surowce", "produkcja"
    surowce = fopen("surowce", "r");
    if (surowce == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    produkcja = fopen("produkcja", "w");
    if (produkcja == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //************************************************

    char znak; int ilosc_znakow;
    ilosc_znakow = 0;
    znak = fgetc(surowce);
    while(1)
    {
        //Obsługa wczytanego znaku
        ilosc_znakow++;
        //printf("%d : ASCII=%d\n", ilosc_znakow, znak); //wyświetla po kolei przerabiane znaki
        //************************

        semafor_czerwone(semid, 0); //TRWA PRODUKCJA (zaraz po zakończeniu odczytu)

        //Zapis znaku do bufora
        towar = fopen("towar", "w");
        if (towar == NULL)
        {
            perror("fopen()");
            exit(1);
        }
        fputc(znak, towar);
        if (fclose(towar) == EOF)
        {
            perror("fclose()");
            exit(1);
        }
        //*********************

        semafor_zielone(semid, 1); //UMOŻLIWIENIE ODCZYTU

        if (znak == EOF) break; //zapisujemy EOF do pliku "towar", aby poinformować o końcu produkcji

        //Dołączenie znaku do całkowitej produkcji (pliku "produkcja")
        fputc(znak, produkcja);
        //************************************************************

        //Pobranie nowego znaku
        znak = fgetc(surowce);
        //*********************

    }

    //Zamknięcie plików roboczych
    if (fclose(surowce) == EOF)
    {
        perror("fclose()");
        exit(1);
    }
    if (fclose(produkcja) == EOF)
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

int utworz_zestaw_semaforow()
{
    /* Get unique key for semaphore. */
	key_t semKey;
	char *path = "/tmp";  //plik musi istnieć
	int id = (int)getpid();
	semKey = ftok(path, id);
	if (semKey == (key_t)-1)
	{
		perror("IPC error: ftok");
		exit(1);
	}
	//*********************************

	/* Get semaphore ID associated with this key. */
	int semId;
	if ((semId = semget(semKey, 0, 0)) == -1)
	{
		/* Semaphore does not exist - Create. */
		if ((semId = semget(semKey, 2/*nsems*/, 0600 | IPC_CREAT | IPC_EXCL)) != -1)
		{
			/*Ustawienie semaforów*/
			if (semctl(semId, 0 , SETVAL, 1) == -1) //towar skonsumowano
			{
				perror("Nadanie wartości semaforowi 0");
				printf("errno=%d\n", errno);
				exit(2);
			}
            if (semctl(semId, 1 , SETVAL, 0) == -1) //towar wyprodukowano
			{
				perror("Nadanie wartości semaforowi 1");
				printf("errno=%d\n", errno);
				exit(2);
			}

		}
	}
	return semId;
}
