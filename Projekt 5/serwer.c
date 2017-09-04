#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h> //toupper()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
//**********

void bypass_sigint(int signum)
{
    printf("Serwer kończy pracę!\n");
    if (msgctl(msqid, IPC_RMID, 0) == -1)
    {
        perror("msgctl() error");
        exit(1);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    //Obsługa sygnału SIGINT - CTRL+C
    printf("Aby zakończyć działanie programu wciśnij Ctrl+C");
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler(&bypass_sigint);
    sigaction(SIGINT, &sa, NULL);
    //*******************************

    //Tworzenie kolejki komunikatów
	key_t msqKey;
	char *path = "/tmp";  //plik musi istnieć
	int id = (int)getpid();
	msqKey = ftok(path, id);
	if (msqKey == (key_t)-1)
	{
		perror("IPC error: ftok");
		exit(1);
	}
	if ((msqid = msgget(msqKey, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
        perror("msgget error");
        exit(1);
	}
	printf("Adres serwera (id kolejki komunikatów): %d\n", msqid);
    //*****************************

    while(1)
    {
        //Odbieranie danych
        memset(&msg_rcv, NULL, sizeof(msg_rcv));
        if (msgrcv(msqid, &msg_rcv, MAX_MSG_SIZE, /*(long)getpid()*/0, 0) == -1) //ostatni parametr = 0 - oczekiwanie
        {
            perror("msgrcv()");
            exit(1);
        }
        //*****************************************

        //To upper
        for (i=0; i<strlen(msg_rcv->mtext); i++)
        {
            msg_rcv->mtext[i] = toupper(msg_rcv->mtext[i]);
        }
        sleep(2);
        //********

        //Przygotowywanie komunikatu
        strcpy(tmpText, msg_rcv->mtext);
        memset(&msg_snd, 0, sizeof(msg_snd));
        msg_snd->mtype = msg_rcv->mtype; //PID odbiorcy
        strcpy(msg_snd->mtext, msg_rcv->mtext);
        //**************************

        //Wyświetlanie komunikatu
        printf("Serwer odebrał komunikat od klienta: %ld\nTreść:\n%s\nPo potraktowaniu go funkcją toupper:\n%s\n", msg_rcv->mtype, tmpText);
        //***********************

        //Wysyłanie odpowiedzi
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
