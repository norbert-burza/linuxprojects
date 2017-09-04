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
int file_descriptor, nr_konsumenta;
//**********

char nazwa_pliku[64];

int main(int argc, char *argv[])
{
    //Obsługa parametrów wywołania
    if (argc != 3)
    {
        printf("Niepoprawna ilość parametrów\n");
        printf("Spróbuj z parametrami: file_descriptor nr_konsumenta\n");
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

        nr_konsumenta = strtol(argv[2], &ptrEnd, 10);
        if (errno == ERANGE || errno == EINVAL)
        {
            printf("Wartość parametru nr_konsumenta jest błędna.\n");
            exit(2);
        }
    }
    //****************************

    memset(nazwa_pliku, 0, sizeof(nazwa_pliku));
    sprintf(nazwa_pliku, "wyjscie%d", nr_konsumenta);

    printf("____________________\nnr_konsumenta=%d\nfile_descriptor=%d\nnazwa_pliku=%s\n", nr_konsumenta, file_descriptor, nazwa_pliku);

    //Otwieranie pliku do zapisu danych
    FILE * wyjscie;
    wyjscie = fopen(nazwa_pliku, "w");
    if (wyjscie == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //*********************************

    char buf;
    int resultOfread;
    for ( ; ; )
    {
        sleep(1);
        resultOfread = read(file_descriptor, &buf, sizeof(buf));
        if (resultOfread > 0)
        {
            //odczytano znak
            if (buf == EOF) continue;
            printf("%c\n", buf);
            fputc(buf, wyjscie);
            fflush(wyjscie);
        }
        else if (resultOfread == 0)
        {
            //brak danych
            continue;
        }
        else if (resultOfread == -1)
        {
            //bład
            perror("read()");
            exit(1);
        }
    }

    //Zamykanie pliku do zapisu danych
    if (fclose(wyjscie) == EOF)
    {
        perror("fclose()");
        exit(1);
    }
    //********************************


}
