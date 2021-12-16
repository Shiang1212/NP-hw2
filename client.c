#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <time.h>

int clientfd2;

char* IP = "127.0.0.1";

short PORT = 6985;

typedef struct sockaddr meng;

char name[30];

time_t nowtime;

void init(){

    clientfd2 = socket(PF_INET,SOCK_STREAM,0);

    struct sockaddr_in addr;

    addr.sin_family = PF_INET;

    addr.sin_port = htons(PORT);

    addr.sin_addr.s_addr = inet_addr(IP);


    if (connect(clientfd2,(meng*)&addr,sizeof(addr)) == -1){

        perror("lost connection");

        exit(-1);

    }

    printf("client activated\n");

}

void start()
{

    pthread_t id;

    void* recv_thread(void*);


    pthread_create(&id, NULL,recv_thread,0);

    char buf2[100] = {};

    sprintf(buf2,"%s enter the chatroom",name);

    time(&nowtime);

    printf("Enter time: %s\n",ctime(&nowtime));

    send(clientfd2,buf2,strlen(buf2),0);

    while(1)
    {

        char buf[1024] = {};
        scanf("%s",buf);

        send(clientfd2,buf,1024,0);

        if (strcmp(buf,"quit") == 0){

            memset(buf2,0,sizeof(buf2));
            sprintf(buf2,"%s log out",name);

            send(clientfd2,buf2,strlen(buf2),0);

            break;
        }
    }

    close(clientfd2);

}

void* recv_thread(void* p){

    while(1){

        char buf[100] = {};

        if (recv(clientfd2,buf,sizeof(buf),0) <= 0){

            break;

        }

        printf("%s\n",buf);

    }

}

int main(){

    init();

    printf("Please enter name：");

    scanf("%s",name);

    printf("\n\n*****************************\n");

    printf("Welcome %s enter the chatroom\n\n",name);

    printf("Enter 'quit' to log out\n");

    printf("\n*****************************\n\n");

    start();

    return 0;

}