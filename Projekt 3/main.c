#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define LICZBA_PP_MAX 5
#define LICZBA_SK_MAX 5

//------ Semaphore Limits --------
const int max_number_of_arrays = 128;
const int max_semaphores_per_array = 250;
const int max_semaphores_system_wide = 32000;
const int max_ops_per_semop_call = 32; //100 na torus.uck.pk.edu.pl
const int semaphore_max_value = 32767;
//--------------------------------

void semctlErrors();
void semafor_czerwone(int);
void semafor_zielone(int);
void usun_semafor(int);

pid_t nowy_pid;

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("Błąd: Niepoprawna ilość argumentów\nNależy podać 3 agrumenty: program_potomny liczba_pp liczba_sk\nliczba_pp - liczba procesów potomnych rywalizujących o sk\nliczba_sk - liczba wykonań przez każdy z procesów potomnych\n");
        exit(1);
	}

	long int liczba_pp, liczba_sk;
	char *program_potomny[50];
	char *pEnd;

	liczba_pp = strtol(argv[2], &pEnd ,10/*int base*/);
	if (errno == ERANGE || liczba_pp > LICZBA_PP_MAX || liczba_pp <= 0) { printf("Błędna wartość liczba_pp\n"); exit(2); }  //liczby ujemne
	liczba_sk = strtol(argv[3], &pEnd ,10);
	if (errno == ERANGE || liczba_sk > LICZBA_SK_MAX || liczba_sk <= 0) { printf("Błędna wartość liczba_sk\n"); exit(2); }

    /*Wypisz parametry wywołania programu*/
	printf("liczba_pp=%ld\nliczba_sk=%ld\n", liczba_pp, liczba_sk);
	printf("argc=%d\nargv[0]=%s\nargv[1]=%s\nargv[2]=%s\nargv[3]=%s\n", argc, argv[0], argv[1], argv[2], argv[3]);

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
	/*********************************/

	/* Get semaphore ID associated with this key. */
	int semId;
	if ((semId = semget(semKey, 0, 0)) == -1)
	{
		/* Semaphore does not exist - Create. */
		if ((semId = semget(semKey, 1, 0777 | IPC_CREAT | IPC_EXCL)) != -1)
		{
			printf("semId=%d\n", semId);

			/*Ustawienie semafora*/
			if (semctl(semId, 0 , SETVAL, 1) == -1)
			{
				perror("Nadanie wartości semaforowi 0");
				printf("errno=%d\n", errno);
				semctlErrors();
				exit(2);
			}

		}
	}

    char * buffer; int n;
    buffer = malloc((size_t)32);
    n = sprintf(buffer, "%d", semId);

	/*Tworzenie procesów*/
	int i, j;
	char *program_potomny_path = malloc((size_t)255); //"./program_potomny";
	program_potomny_path[0] = '.';
	program_potomny_path[1] = '/';
        for (j=0; j<((int)strlen(argv[1])); j++)
	{
		program_potomny_path[j+2] = argv[1][j];
	}
	for (i=0; i<liczba_pp; i++)
	{
		switch(fork())
		{
			case -1:
			perror("fork() error");
			exit(1);
				break;
			case 0:
			if (execl(program_potomny_path, argv[1], argv[3], buffer, NULL) == -1)
			{
				perror("execl() error");
				exit(1);
			}
				break;
		}
	}

	int status;
	for (i=0; i<liczba_pp; i++)
	{
		nowy_pid = wait(&status);
	}
        usun_semafor(semId);
}

void semafor_zielone(int semId)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = 0; /* semaphore number */
	sembuf_struct.sem_op = 1; /* semaphore operation */
	sembuf_struct.sem_flg = SEM_UNDO; /* operation flags */
	if (semop(semId, &sembuf_struct, 1) == -1)
	{
		perror("semop");
		exit(3);
	}
}

void semafor_czerwone(int semId)
{
	struct sembuf sembuf_struct;
	sembuf_struct.sem_num = 0; /* semaphore number */
	sembuf_struct.sem_op = -1; /* semaphore operation */
	sembuf_struct.sem_flg = SEM_UNDO; /* operation flags */
	if (semop(semId, &sembuf_struct, 1) == -1)
	{
		perror("semop");
		exit(3);
	}
}

void usun_semafor(int semid)
{
	int result;
	if ((result=semctl(semid, 0, IPC_RMID)) == -1)
	{
		perror("usun_semafor - semctl");
		exit(1);
	}
}

/*Wypisywanie informacji o kodzie błędu*/
void semctlErrors()
{
	printf("/***/\nEACCES=%d\n",EACCES);
	printf("EFAULT=%d\n",EFAULT);
	printf("EIDRM=%d\n",EIDRM);
	printf("EINVAL=%d\n",EINVAL);
	printf("EPERM=%d\n",EPERM);
	printf("ERANGE=%d\n/***/\n",ERANGE);
}
