#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MAX_MSG_SIZE 256

//Deklaracje
int i, msqid;
char tmpText[MAX_MSG_SIZE];
struct msgbuf {
    long mtype;                /* message type, must be > 0 */
    char mtext[MAX_MSG_SIZE];  /* message data */
};
struct msgbuf msg_snd;
struct msgbuf msg_rcv;
pthread_t rcv_thread;
//**********

//Obsługa sygnału Terminal interrupt signal (Ctrl+C)
void bypass_sigint(int signum)
{
    pthread_cancel(rcv_thread);
    exit(0);
}
//**************************************************

int main(int argc, char *argv[])
{
    //Wczytywanie parametrów
    if (argc == 2)
    {
        int id = atoi(argv[1]);
        //Tworzenie kolejki komunikatów
        key_t msqKey;
        char *path = "/tmp";  //plik musi istnieć
        msqKey = ftok(path, id);
        if (msqKey == (key_t)-1)
        {
            perror("IPC error: ftok");
            exit(1);
        }
        if ((msqid = msgget(msqKey, 0600)) == -1)
        {
            if (errno == ENOENT)
            {
                printf("Brak kolejki komunikatów o takim identyfikatorze!\n");
                exit(1);
            }
            perror("msgget error");
            exit(1);
        }
        printf("Adres kolejki komunikatów: %d\n", msqid);
        //*****************************
    }
    else
    {
        printf("Jako parametr należy podać: PID serwera\n");
        exit(1);
    }
    //**********************

    //Obsługa sygnału SIGINT - CTRL+C
    printf("Aby zakończyć działanie programu wciśnij Ctrl+C");
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler(&bypass_sigint);
    sigaction(SIGINT, &sa, NULL);
    //*******************************

    //Utwórz wątek do odbierania komunikatów
    if (pthread_create(%rcv_thread, NULL, Msg_rcv, NULL) != 0)
    {
        perror("pthread_create()");
        exit(2);
    }
    //**************************************

    while(1)
    {
        //Wczytanie danych ze standardowego wejścia
        printf("Wprowadź tekst (koniec tekstu Ctrl+D):\n");
        scanf("%s", tmpText);
        //*****************************************

        //Przygotowywanie komunikatu
        memset(&msg_snd, 0, sizeof(msg_snd));
        msg_snd->mtype = (long)getpid();
        strcpy(msg_snd->mtext, tmpText);
        //**************************

        //Wysyłanie komunikatu
        if (msgsnd(msqid, &msg_snd, (size_t)strlen(msg_snd->mtext), IPC_NOWAIT) == -1)
        {
            if (errno == EAGAIN)
            {
                printf("Brak miejsca w kolejce komunikatów!\nKomunikat nie został wysłany.\n");
                exit(1);
            }
            perror("msgsnd() error");
            exit(1);
        }
        //********************
    }
}

//Odbieranie komunikatów z kolejki
void Msg_rcv()
{
    while(1)
    {
        //Odbieranie odpowiedzi od serwera
        memset(&msg_rcv, 0, sizeof(msg_rcv));
        if (msgrcv(msqid, &msg_rcv, MAX_MSG_SIZE, (long)getpid(), 0) == -1) //ostatni parametr = 0 - oczekiwanie
        {
            perror("msgrcv()");
            exit(1);
        }
        printf("Odebrano odpowiedź:\n%s\n", msg_rcv->mtext);
        //********************************
    }
}
//********************************
