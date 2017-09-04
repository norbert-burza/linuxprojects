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
        printf("Spróbuj z parametrami: nazwa_wyjscia\n");
        exit(1);
    }
    //****************************

    //Otwieranie pliku do zapisu danych
    FILE * wyjscie;
    wyjscie = fopen(argv[1], "w");
    if (wyjscie == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    //*********************************

    //Należy odczytać dane z FIFO i zapisać do pliku wskazanego przez argv[1]

    //FIFO
    int file_descriptor = open("plik_fifo", O_RDONLY | O_NONBLOCK);
    if (file_descriptor == -1)
    {
        perror("plik_fifo open()");
        exit(1);
    }
    //****
    char buf;
    int resultOfread;
    for ( ; ; )
    {
        sleep(1);
        resultOfread = read(file_descriptor, &buf, sizeof(buf));
        if (resultOfread > 0)
        {
            //odczytano znak
            if (buf == EOF)
            {
                printf("Koniec pliku\n");
                continue;
            }
            //printf("%c\n", buf);
            fputc(buf, wyjscie);
            fflush(wyjscie);

        }
        else if (resultOfread == 0)
        {
            //brak danych
            continue;
            break;
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
