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

int main(int argc, char *argv[])
{
    //Obsługa parametrów wywołania
    if (argc != 2)
    {
        printf("Niepoprawna ilość parametrów\n");
        printf("Spróbuj z parametrami: nazwa_wejscia\n");
        exit(1);
    }
    //****************************

    //Otwieranie pliku z danymi (wejścia)
    FILE * wejscie;
    wejscie = fopen(argv[1], "r");
    if (wejscie == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //***********************************

    //Tworzenie FIFO
    if (mkfifo("plik_fifo", 0700) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo()");
            exit(1);
        }
    }
    int file_descriptor = open("plik_fifo", O_WRONLY);
    if (file_descriptor == -1)
    {
        perror("open()");
        exit(1);
    }
    //**************

    //Czytanie pliku wejscia znak po znaku
    char znak;
    int ilosc_znakow;
    ilosc_znakow = 0;
    while(1)
    {
        znak = fgetc(wejscie);
        if (write(file_descriptor, &znak, sizeof(char)) != 1)
        {
            printf("blad w zapisie danych do FIFO\n");
            exit(1);
        }
        if (znak == EOF) break;
        ilosc_znakow++;
        printf("%d : ASCII %d\n", ilosc_znakow, znak);

    }
    //************************************

    //Zwalnianie zasobów
    if (fclose(wejscie) == EOF)
    {
        perror("fclose()");
        exit(1);
    }
    close(file_descriptor);
    //******************
}
