#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*  INPUT_MODE:

    1 : server_msg
    2 : client_ msg
    3 : invite_play
    4 : response_play  
    5 : client_list   */

struct client
{
  char ID[16];
  char account[16];
  char password[16];
};

char online_list[5][16]={{0},{0},{0},{0},{0}};



struct client client_1 = {{'S','H','A','I','N','G','\0'}, {'A','B','C','\0'}, {'1','2','3','\0'}};

struct client client_2 = {{'C','H','A','N','G','\0'}, {'D','E','F','\0'}, {'4','5','6','\0'}};

int mode_check(char input[])
{
  if(input[0] == '/' && (input[2] == ' ' || input[2] == '\n'))
  {
    if(input[1] == 'M')
    {
      return 2;
    }
    else if(input[1] == 'I')
    {
      return 3;
    }
    else if(input[1] == 'R')
    {
      return 4;
    }
    else if(input[1] == 'L')
    {
      return 5;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return 1;
  }
}

void insert_list(char *buffer)
{
  for(int i = 0; i < 5; i++)
  {
    if(online_list[i][0] != ' ')
    {
      strcpy(online_list[i], buffer);
      return;
    }
  }
}

int login_check(char account[], char password[], int fd, char *buffer, int flag)
{
  if(strcmp(account, "ABC") == 0 && strcmp(password, "123") == 0)
  {
    strcpy(buffer, "correct");
    send(fd, buffer, 1024, flag);
    strcpy(buffer, client_1.ID);
    send(fd, buffer, 1024, flag);
    insert_list(client_1.ID);
    return 1;
  }
  else if(strcmp(account, "DEF") == 0 && strcmp(password, "456") == 0)
  {
    strcpy(buffer, "correct");
    send(fd, buffer, 1024, flag);
    strcpy(buffer, client_2.ID);
    send(fd, buffer, 1024, flag);
    insert_list(client_2.ID);
    return 2;
  }
  else
  {
    strcpy(buffer, "wrong");
    send(fd, buffer, 1024, flag);
    return -1;
  }
}

void server_message(int fd, char *buffer, int flag)
{
  send(fd, buffer, 1024, flag);
  return;
}

void client_message()
{
  return;
}

void invite_play()
{
  return;
}

void response_play()
{
  return;
}

void input_error(int fd, char *buffer, int flag)
{
  strcpy(buffer, "Server: Input error!!\n");
  send(fd, buffer, 1024, flag);
  bzero(&buffer, sizeof(buffer));
  return;
}

void client_list(int fd, char *buffer ,int flag)
{
  strcpy(buffer, "\n--Online list:--\n"); 
  for(int i = 0; i < 5; i++)
  {
    strcat(buffer, "-");
    strcat(buffer, online_list[i]);
    strcat(buffer, "\n");
  }
  send(fd, buffer, 1024, flag);
  printf("%s", buffer);
  return;
}

int main()
{
  int welcomeSocket, fd, portNum, clientLen, flag;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  int i;

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  portNum = 46888;
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portNum);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(listen(welcomeSocket, 5) == 0) /*  maximum connection */
    printf("Listening\n");
  else
    printf("Error\n");

  addr_size = sizeof serverStorage;

      int mode;
      flag = 1;
  /*loop to keep accepting new connections*/
  while(1)
  {
    fd = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    /*fork a child process to handle the new connection*/
    printf("Someone try to Login!!\n");
    char login_acc[16], login_pwd[16];
    if(!fork())
    {
      do
      {
        bzero(&login_acc, strlen(login_acc));
        bzero(&login_pwd, strlen(login_pwd));
        bzero(&buffer, strlen(buffer));
        if(recv(fd, buffer, 1024, 0) == -1)
        {
          return -1;
        }
        strcpy(login_acc, buffer);
        login_acc[strlen(login_acc) - 1] = '\0';
        printf("login account = %s\n", login_acc);
        if(recv(fd, buffer, 1024, 0) == -1)
        {
          return -1;
        }
        strcpy(login_pwd, buffer);
        login_pwd[strlen(login_pwd) - 1] = '\0';
        printf("login password = %s\n", login_pwd);
      }while(login_check(login_acc, login_pwd, fd, buffer, flag) == -1);
      printf("Client login successed\n");
      
      /*loop while connection is live*/
      while(flag != 0)
      {
        flag = recv(fd, buffer, 1024, 0);
        printf("client: %s", buffer);
        mode = mode_check(buffer);
        printf("mode: %d\n", mode);
        if(mode == 1)
        {
          server_message(fd, buffer, flag);
        }
        else if(mode == 2)
        {
          client_message();
        }
        else if(mode == 3)
        {
          invite_play();
        }
        else if(mode == 4)
        {
          response_play();
        }
        else if(mode == 5)
        {
          client_list(fd, buffer, flag);
        }
        else
        {
          input_error(fd, buffer, flag);
        }
      }
      close(fd);
      exit(0);
    }
    /*if parent, close the socket and go back to listening new requests*/
    else{
      close(fd);
    }
  }

  return 0;
}