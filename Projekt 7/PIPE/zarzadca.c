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
#include <fcntl.h>
#include <string.h>

#define ILOSC_PRODUCENTOW_MAX 5
#define ILOSC_KONSUMENTOW_MAX 5
#define ILOSC_ITERACJI_MAX 5

//Deklaracje
int ilosc_producentow, ilosc_konsumentow;
int i;
char buf[32];
char i_buf[8];
char fd_buf[8];
//**********
int main(int argc, char *argv[])
{
    //Obsługa parametrów wywołania
    if (argc != 3)
    {
        printf("Niepoprawna ilość parametrów\n");
        printf("Spróbuj z parametrami: ilosc_producentow ilosc_konsumentow\n");
        exit(1);
    }
    else
    {
        char * ptrEnd;
        ilosc_producentow = strtol(argv[1], &ptrEnd, 10);
        if (errno == ERANGE || errno == EINVAL || ilosc_producentow <= 0 || ilosc_producentow > ILOSC_PRODUCENTOW_MAX)
        {
            printf("Wartość parametru ilosc_producentow jest błędna. 0 < ilosc_producentow < ILOSC_PRODUCENTOW_MAX=5\n");
            exit(2);
        }
        ilosc_konsumentow = strtol(argv[2], &ptrEnd, 10);
        if (errno == ERANGE || errno == EINVAL || ilosc_konsumentow <= 0 || ilosc_konsumentow > ILOSC_KONSUMENTOW_MAX)
        {
            printf("Wartość parametru ilosc_konsumentow jest błędna. 0 < ilosc_konsumentow < ILOSC_KONSUMENTOW_MAX=5\n");
            exit(2);
        }
    }
    //****************************

    //Utwórz potok nienazwany
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }
    //***********************

    memset(fd_buf, 0, sizeof(fd_buf));
    sprintf(fd_buf, "%d", pipefd[1]); //"pipefd[1] refers to the write end of the pipe"

    printf("ilosc_producentow=%d  ilosc_konsumentow=%d\npipefd[0]=%d  pipefd[1]=%d\n", ilosc_producentow, ilosc_konsumentow, pipefd[0], pipefd[1]);

    //Utwórz procesy producentów
    for (i=0; i<ilosc_producentow; i++)
    {
        memset(i_buf, 0, sizeof(i_buf));
        sprintf(i_buf, "%d", i);
        switch (fork())
        {
            case -1:
            perror("fork()");
            exit(1);
                break;
            case 0:
            if (execl("./producent", "producent", fd_buf, i_buf, NULL) == -1)
            {
                perror("execl()");
                exit(2);
            }
                break;
            default:
                break;
        }
    }
    //**************************

    memset(fd_buf, 0, sizeof(fd_buf));
    sprintf(fd_buf, "%d", pipefd[0]); //"pipefd[0] refers to the read end of the pipe"

    //Utwórz procesy konsumentów
    for (i=0; i<ilosc_konsumentow; i++)
    {
        memset(i_buf, 0, sizeof(i_buf));
        sprintf(i_buf, "%d", i);
        switch (fork())
        {
            case -1:
            perror("fork()");
            exit(1);
                break;
            case 0:
            if (execl("./konsument", "konsument", fd_buf, i_buf, NULL) == -1)
            {
                perror("execl()");
                exit(2);
            }
                break;
            default:
                break;
        }
    }
    //**************************

    close(pipefd[0]);close(pipefd[1]); //close unused write- and read end

    //Poczekaj na zakończenie producentów i konsumentów
    int status;
    for (i=0; i<(ilosc_producentow+ilosc_konsumentow); i++)
    {
        wait(&status);
    }

    //*************************************************
}
