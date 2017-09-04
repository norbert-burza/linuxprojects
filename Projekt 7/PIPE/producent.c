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

//Deklaracje
int file_descriptor, nr_producenta;
//**********

char nazwa_pliku[256];

int main(int argc, char *argv[])
{
    //Obsługa parametrów wywołania
    if (argc != 3)
    {
        printf("Niepoprawna ilość parametrów\n");
        printf("Spróbuj z parametrami: file_descriptor nr_producenta\n");
        exit(1);
    }
    else
    {
        char * ptrEnd;

        file_descriptor = strtol(argv[1], &ptrEnd, 10);
        if (errno == ERANGE || errno == EINVAL)
        {
            printf("Problem z wartością parametru file_descriptor\n");
            exit(2);
        }

        nr_producenta = strtol(argv[2], &ptrEnd, 10);
        if (errno == ERANGE || errno == EINVAL)
        {
            printf("Wartość parametru nr_producenta jest błędna.\n");
            exit(2);
        }
    }
    //****************************
    memset(nazwa_pliku, 0, sizeof(nazwa_pliku));
    sprintf(nazwa_pliku, "wejscie%d", nr_producenta);

    printf("____________________\nnr_producenta=%d\nfile_descriptor=%d\nnazwa_pliku=%s\n", nr_producenta, file_descriptor, nazwa_pliku);

    //Otwieranie pliku z danymi (wejścia)
    FILE * wejscie;
    wejscie = fopen(nazwa_pliku, "r");
    if (wejscie == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //***********************************

    //Czytanie pliku wejscia znak po znaku
    char znak;
    int ilosc_znakow;
    ilosc_znakow = 0;
    while(1)
    {
        znak = fgetc(wejscie);
        if (write(file_descriptor, &znak, sizeof(char)) != 1)
        {
            printf("blad w zapisie danych do PIPE\n");
            exit(1);
        }
        if (znak == EOF) break;
        ilosc_znakow++;
        //printf("%d : ASCII %d\n", ilosc_znakow, znak);

    }
    //************************************

    //Zwalnianie zasobów
    if (fclose(wejscie) == EOF)
    {
        perror("fclose()");
        exit(1);
    }
    //******************
}
